from PIL import Image, ImageDraw, ImageFont
import os

# Create output directories if they don't exist
os.makedirs("assets/fonts", exist_ok=True)

# Atlas dimensions
atlas_width = 256
atlas_height = 256

# Create a new image with white background
atlas = Image.new('RGBA', (atlas_width, atlas_height), (0, 0, 0, 0))
draw = ImageDraw.Draw(atlas)

# Try to use a system font
try:
    # Try different system fonts
    potential_fonts = [
        'consolas.ttf',
        'arial.ttf',
        'C:/Windows/Fonts/consolas.ttf',
        'C:/Windows/Fonts/arial.ttf',
        'C:/Windows/Fonts/segoeui.ttf',
        'C:/Windows/Fonts/calibri.ttf'
    ]
    
    font = None
    for font_path in potential_fonts:
        try:
            font = ImageFont.truetype(font_path, 20)
            print(f"Using font: {font_path}")
            break
        except:
            continue
    
    if font is None:
        font = ImageFont.load_default()
        print("Using default font")
except Exception as e:
    print(f"Error loading font: {e}")
    font = ImageFont.load_default()
    print("Using default font")

# Characters to include
chars = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"

# Calculate grid layout
char_width = 16
char_height = 24
cols = atlas_width // char_width
rows = atlas_height // char_height

# Draw characters on atlas
for i, char in enumerate(chars):
    if i >= rows * cols:
        break
        
    row = i // cols
    col = i % cols
    
    x = col * char_width
    y = row * char_height
    
    # Draw character with white color
    draw.rectangle([x, y, x+char_width-1, y+char_height-1], outline=(64, 64, 64, 255))
    draw.text((x+2, y+2), char, fill=(255, 255, 255, 255), font=font)

# Save the atlas
atlas.save("assets/fonts/consolas.png")
print(f"Font atlas saved to assets/fonts/consolas.png")

# The .fnt file already exists, so we don't need to create it again
print("Done! Now copy the atlas to the build directory") 