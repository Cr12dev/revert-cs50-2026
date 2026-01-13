from PIL import Image, ImageDraw

img = Image.new('RGB', (400,300), color='navy')
draw = ImageDraw.Draw(img)

draw.rectangle([50, 50, 350, 250], fill='lightblue', outline='white', width=3)
draw.ellipse([150, 100, 250, 200], fill='gold', outline='red', width=2)
draw.text((175, 140), "SDL2", fill='black')

img.save('foto.bmp')
print("Imagen de prueba creada: foto.bmp")