#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <asm/io.h>
#include "address_map_arm.h"

#define DEVICE_NAME "gpucjjg"

static int major;
static unsigned char device_buffer[8];
static int device_open = 0;

// Endereços físicos
#define DATA_A 0x80 // Registrador com opcode e endereçamentos
#define DATA_B 0x70 // Registradores de dados
#define START 0xc0  // WRREG
#define WRFULL 0xb0

// Ponteiros essenciais
volatile long long *pointer_dataA;
volatile long long *pointer_dataB;
volatile long long *pointer_START;
volatile long long *pointer_fifo_FULL;
void *pointer_bridge;


static int dev_open(struct inode *inodep, struct file *filep) {
    if (device_open) {
        return -EBUSY;
    }
    device_open++;
    try_module_get(THIS_MODULE);
    printk(KERN_INFO "Dispositivo aberto com sucesso!\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    device_open--;
    module_put(THIS_MODULE);
    printk(KERN_INFO "Dispositivo fechado com sucesso!\n");
    return 0;
}

unsigned long long charArrayToInt(const char *array) {
    unsigned long long result = 0;

    // Itera sobre os chars do array
    int i;
    for (i = 0; i < 8; i++) {
        // Adiciona 8 bits do char atual ao resultado
        result |= ((unsigned long long)(array[i] & 0xFF) << (i * 8));
    }
    
    printk(KERN_INFO "Array to long: %d\n", result);

    return result;
}

void printBinaryArray(const unsigned char *array) {
    printk(KERN_INFO "Array na lib:\n");

    int i, j;
    for (i = 7; i >= 0; i--) {  // Invertendo a ordem dos bytes
        for (j = 7; j >= 0; j--) {
            printk(KERN_INFO "%d", (array[i] >> j) & 1);
        }
        printk(KERN_INFO " ");
    }
    printk(KERN_INFO "\n");
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {

    // Copiar os dados do buffer do usuário para o buffer do dispositivo
    if (copy_from_user(device_buffer, buffer, len) != 0) {
        printk(KERN_ERR "Falha ao copiar dados do buffer de usuário para o buffer do dispositivo\n");
        return -EFAULT;
    }    
   	
    printk(KERN_INFO "\n");
    printk(KERN_INFO "\n");

    // Recuper a instrucao
    unsigned long long word = charArrayToInt(device_buffer);
    printBinaryArray(device_buffer);

    pointer_dataA = (volatile long long *)(pointer_bridge + DATA_A);
    pointer_dataB = (volatile long long *)(pointer_bridge + DATA_B);
    pointer_START = (volatile long long *)(pointer_bridge + START);
    pointer_fifo_FULL = (volatile long long *)(pointer_bridge + WRFULL);

    unsigned long long dataA = 4;
    unsigned long long dataB = 0;

    switch (word & 0b1111)
    {
    case 0b0000:
        dataA = word & ((1 << 9) - 1);
        dataB = word >> 9;
        break;

    case 0b0001:
        dataA = word & ((1 << 18) - 1);
        dataB = word >> 18;
        break;

    case 0b0010:
        dataA = word & ((1 << 17) - 1);
        dataB = word >> 17;
        break;
        
    case 0b0011:
        dataA = word & ((1 << 8) - 1);
        dataB = word >> 8;
        break;

    default:
        printk(KERN_ERR "kernelcjjg: Instrução não reoconhecida");
        break;
    }

    while (*pointer_fifo_FULL) {
        continue;
    }

    // Insere no barramento
    *pointer_dataA = dataA;
    *pointer_dataB = dataB;

    // Autoriza a inserção nas FIFOs
    *pointer_START = 1;
    *pointer_START = 0; 

    // Limpa o buffer do dispositivo
    memset(buffer, 0, sizeof(buffer));

    // Debug
    printk(KERN_INFO "Dados escritos no dispositivo!\n");
    return len;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    // Verificar se o tamanho do buffer de saída é o esperado
    if (len != sizeof(device_buffer)) {
        printk(KERN_ERR "Tamanho do buffer de saída incorreto\n");
        return -EINVAL;
    }

    // Copiar os dados do buffer do dispositivo para o buffer do usuário
    if (copy_to_user(buffer, device_buffer, len) != 0) {
        printk(KERN_ERR "Falha ao copiar dados do buffer do dispositivo para o buffer do usuário\n");
        return -EFAULT;
    }

    printk(KERN_INFO "Dados lidos do dispositivo!\n");
    return len;
}

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .write = dev_write,
    .read = dev_read,
};

static int __init driver_gpu_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Falha ao registrar o dispositivo com o número major %d\n", major);
        return major;
    }
    printk(KERN_INFO "Dispositivo registrado com sucesso com o número major %d. Para interagir com o driver, crie um arquivo de dispositivo com 'mknod /dev/%s c %d 0'.\n", major, DEVICE_NAME, major);
    
    // Mapear o endereço físico para um endereço virtual
    pointer_bridge = ioremap(LW_BRIDGE_BASE, LW_BRIDGE_SPAN);
    
    return 0;
}

static void __exit driver_gpu_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Driver removido com sucesso!\n");
}

module_init(driver_gpu_init);
module_exit(driver_gpu_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Seu Nome");
MODULE_DESCRIPTION("Um driver simples de caractere");
MODULE_VERSION("0.1");
