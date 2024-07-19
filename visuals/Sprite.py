from PIL import Image

# Load the new image
a = input("nome da imagem: ")
b = input("Numero: ")
image_path = f"D:/GABRIEL/UEFS/2024.1/MI-Sistemas_Digitais/Problema-3/visuals/{a}.png"
image = Image.open(image_path)

# Convert the image to RGB and resize it to fit into the 80x60 matrix if necessary
image_rgb = image.convert("RGB")

# Quantize the image to 8 levels per channel (0 to 7)
def quantize(value):
    return value // 32  # 256 / 8 = 32, so divide by 32 to get values in range 0-7

quantized_image = image_rgb.point(quantize)

# Get the pixel data
pixels = quantized_image.load()

# Generate the C code for WBM and WBR_BG
instructions = []

# Assuming the background is the color of the top-left pixel
background_color = pixels[0, 0]

# Generate WBM instructions for each pixel
for y in range(18):
    for x in range(20):
        color = pixels[x, y]
        if color != background_color:
            position = y * 20 + x
            instructions.append(f"WSM({b}, {position}, {color[0]}, {color[1]}, {color[2]});")
        else:
            position = y * 20 + x
            instructions.append(f"WSM({b}, {position}, 6, 7, 7);")



# Combine instructions into a single string
instructions_code = "\n".join(instructions)
print(instructions_code)
input()
