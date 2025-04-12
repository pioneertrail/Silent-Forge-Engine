# PowerShell script to create a 2x2 PNG file
Add-Type -AssemblyName System.Drawing

# Create a bitmap
$bitmap = New-Object System.Drawing.Bitmap 2, 2

# Set pixel colors
$bitmap.SetPixel(0, 0, [System.Drawing.Color]::Red)     # Red
$bitmap.SetPixel(1, 0, [System.Drawing.Color]::Green)   # Green
$bitmap.SetPixel(0, 1, [System.Drawing.Color]::Blue)    # Blue
$bitmap.SetPixel(1, 1, [System.Drawing.Color]::Yellow)  # Yellow

# Save as PNG
$bitmap.Save("assets/textures/test.png", [System.Drawing.Imaging.ImageFormat]::Png)
$bitmap.Dispose()

Write-Host "PNG file created successfully!" 