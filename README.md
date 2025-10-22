# pak-adventure

A PAK utility for Quake II-compatible `.pak` files.

## Features

- Gallery view of the image files inside the `.pak` archive.
    - PCX images are supported and should just work™ if they are the types most commonly used in games like Quake, Quake II, DOOM, etc:
        - It uses run-length encoding (RLE), which almost all PCX images do
        - It uses a 256-color palette
        - It uses 8-bit single-plane
    - Currently the following are unsupported:
        - 4-bit (16-color) PCX images (EGA/VGA)
        - 24-bit "true color" PCX images
        - Monochrome / 1-bit PCX images
        - Uncompressed PCX (i.e no RLE)
