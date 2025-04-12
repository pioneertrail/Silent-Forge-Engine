#!/usr/bin/env python3
"""
Font Atlas Generator
A simple script to generate a bitmap font texture atlas for use with OpenGL text rendering.
"""

from PIL import Image, ImageDraw, ImageFont
import os
import sys

def generate_font_atlas(output_path, font_size=32, grid_size=(16, 6), char_size=(32, 32)):
    """
    Generate a bitmap font texture atlas.
    
    Args:
        output_path (str): Path to save the output image
        font_size (int): Font size to use
        grid_size (tuple): Number of columns and rows in the grid
        char_size (tuple): Size of each character cell in pixels
    """
    cols, rows = grid_size
    char_width, char_height = char_size
    
    # Create image with a grid of cells for characters
    width = cols * char_width
    height = rows * char_height
    
    # Create a white image with transparent background
    image = Image.new('RGBA', (width, height), (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    # Try to load a font, fall back to default if not available
    try:
        # Try to find a system font that should be available on most systems
        if os.name == 'nt':  # Windows
            font = ImageFont.truetype("arial.ttf", font_size)
        else:  # macOS, Linux
            font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", font_size)
    except IOError:
        print("Warning: System font not found, using default bitmap font")
        font = ImageFont.load_default()
    
    # Define character set (ASCII printable characters)
    start_char = 32  # Space
    end_char = 126   # Tilde
    charset = ''.join(chr(i) for i in range(start_char, end_char + 1))
    
    # Draw each character in its grid cell
    for i, char in enumerate(charset):
        if i >= cols * rows:
            break  # Don't draw more characters than we have cells
            
        col = i % cols
        row = i // cols
        
        # Calculate position
        x = col * char_width
        y = row * char_height
        
        # Draw character background (optional)
        # draw.rectangle([x, y, x + char_width, y + char_height], outline=(200, 200, 200, 128))
        
        # Calculate text position to center in cell
        text_width, text_height = draw.textbbox((0, 0), char, font=font)[2:4]
        text_x = x + (char_width - text_width) // 2
        text_y = y + (char_height - text_height) // 2
        
        # Draw character
        draw.text((text_x, text_y), char, font=font, fill=(255, 255, 255, 255))
    
    # Ensure the output directory exists
    os.makedirs(os.path.dirname(os.path.abspath(output_path)), exist_ok=True)
    
    # Save the image
    image.save(output_path)
    print(f"Font atlas generated and saved to {output_path}")
    print(f"Character set: {charset}")
    print(f"Grid: {cols}x{rows}, Character cell size: {char_width}x{char_height}")

if __name__ == "__main__":
    output_dir = "textures"
    output_file = "font_atlas.png"
    output_path = os.path.join(output_dir, output_file)
    
    # Ensure the output directory exists
    os.makedirs(output_dir, exist_ok=True)
    
    # Generate the font atlas
    generate_font_atlas(output_path)
    
    print(f"To use this font with the TextRenderer, call:")
    print(f"textRenderer.initialize(")
    print(f"    \"textures/{output_file}\",")
    print(f"    32, 32,   // Character width and height")
    print(f"    16, 6,    // Grid size (columns, rows)")
    print(f"    \" !\\\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\\\]^_`abcdefghijklmnopqrstuvwxyz{{|}}~\"")
    print(f");") 