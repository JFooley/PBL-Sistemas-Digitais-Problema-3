#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>

#define LW_BRIDGE_BASE 0xFF200000
#define LW_BRIDGE_SPAN 0x00005000
#define KEYS_BASE 0x0

#define HEX5_BASE 0x10  
#define HEX4_BASE 0x20
#define HEX3_BASE 0x30
#define HEX2_BASE 0x40
#define HEX1_BASE 0x50
#define HEX0_BASE 0x60

/* Prototypes for functions used to access p memory addresses */
int open_p (int);
void * map_p (int, unsigned int, unsigned int);
int unmap_p (void *, unsigned int);
void close_p (int);

int get_key(int value) {
    int key_value = 0;
    int fd = -1;
    
    void *LW_virtual;
    volatile int * key_button;

    if ((fd = open_p(fd)) == -1) {
        perror("Erro ao abrir o FD do botao");
        return 1;
    }
    if (!(LW_virtual = map_p(fd, LW_BRIDGE_BASE, LW_BRIDGE_SPAN))) {
        perror("Erro ao mapear a LW BRIDGE");
        return (-1);
    }

    key_button = (unsigned int *) (LW_virtual + KEYS_BASE);

    key_value = (*key_button == value);

    unmap_p(LW_virtual, LW_BRIDGE_SPAN);
    close(fd);
    return key_value;
}

int decode(int numero){

    int binario;

    switch(numero){
        case 0:
            binario = 0b1000000;
        break;

        case 1:
            binario = 0b1111001;
        break;

        case 2:
            binario = 0b0100100;
        break;

        case 3:
            binario = 0b0110000;
        break;

        case 4:
            binario = 0b0011001;
        break;

        case 5:
            binario = 0b0010010;
        break;

        case 6:
            binario = 0b0000010;
        break;

        case 7:
            binario = 0b1111000;
        break;

        case 8:
            binario = 0b0000000;
        break;

        case 9:
            binario = 0b0011000;
        break;

        default: binario = 0b1111111;
    }

    return binario;

}

int set_display(int numero, int digito) {
 	volatile int * DISPLAY_ptr0; // virtual address pointer to red LEDs
    volatile int * DISPLAY_ptr1; // virtual address pointer to red LEDs
    volatile int * DISPLAY_ptr2; // virtual address pointer to red LEDs
    volatile int * DISPLAY_ptr3; // virtual address pointer to red LEDs
    volatile int * DISPLAY_ptr4; // virtual address pointer to red LEDs
    volatile int * DISPLAY_ptr5; // virtual address pointer to red LEDs

    int fd = -1; // used to open /dev/mem
    void *LW_virtual; // p addresses for light-weight bridge

	// Create virtual memory access to the FPGA light-weight bridge
    if ((fd = open_p (fd)) == -1)
    return (-1);
    if (!(LW_virtual = map_p (fd, LW_BRIDGE_BASE, LW_BRIDGE_SPAN)))
    return (-1);

    //Definindo ponteiros de acordo com endereço
    DISPLAY_ptr0 = (unsigned int *) (LW_virtual + HEX0_BASE);
    DISPLAY_ptr1 = (unsigned int *) (LW_virtual + HEX1_BASE);
    DISPLAY_ptr2 = (unsigned int *) (LW_virtual + HEX2_BASE);
    DISPLAY_ptr3 = (unsigned int *) (LW_virtual + HEX3_BASE);
    DISPLAY_ptr4 = (unsigned int *) (LW_virtual + HEX4_BASE);
    DISPLAY_ptr5 = (unsigned int *) (LW_virtual + HEX5_BASE);

    int binario = decode(numero);

    switch (digito)
    {
        case 0:
            *DISPLAY_ptr0 = decode(numero);
        break;
        
        case 1:
            *DISPLAY_ptr1 = decode(numero);
        break;

        case 2:
            *DISPLAY_ptr2 = decode(numero);
        break;
        
        case 3:
            *DISPLAY_ptr3 = decode(numero);
        break;

        case 4:
            *DISPLAY_ptr4 = decode(numero);
        break;

        case 5:
            *DISPLAY_ptr5 = decode(numero);
        break;

        default:
        break;
    }
   
	unmap_p (LW_virtual, LW_BRIDGE_SPAN);
    close_p (fd);
    return 0;
}

/* Open /dev/mem to give access to p addresses */
int open_p (int fd) {
    if (fd == -1) // check if already open
    if ((fd = open( "/dev/mem", (O_RDWR | O_SYNC))) == -1) {
    printf ("ERROR: could not open \"/dev/mem\"...\n");
    return (-1);
}
    return fd;

}

/* Close /dev/mem to give access to p addresses */
void close_p (int fd) {
    close (fd);
}

/* Establish a virtual address mapping for the p addresses starting
* at base and extending by span bytes */
void* map_p(int fd, unsigned int base, unsigned int span) {
    void *virtual_base;

    // Get a mapping from p addresses to virtual addresses
    virtual_base = mmap (NULL, span, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, base);

    if (virtual_base == MAP_FAILED) {
        printf ("ERROR: mmap() failed\n");
        close (fd);
        return (NULL);
    }

    return virtual_base;
}

/* Close the previously-opened virtual address mapping */
int unmap_p(void * virtual_base, unsigned int span) {
    if (munmap (virtual_base, span) != 0) {
        printf ("ERROR: munmap() failed\n");
        return (-1);
    }

    return 0;
}
