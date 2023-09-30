import os
import sys
import cv2
import numpy as np
import matplotlib.pyplot as plt


# Check if the correct number of command-line arguments is provided
if len(sys.argv) != 2:
    print("Usage: python script_name.py folder_name")
    sys.exit(1)

# Get the folder name from command-line arguments
folder_name = sys.argv[1]
folder_path = f'../output/{folder_name}'
output_folder_path = f'../plot/{folder_name}'
# List all files in the folder
file_names = os.listdir(folder_path)

# Process each file in the folder
for file_name in file_names:
    file_path = os.path.join(folder_path, file_name)

    # Read data from file
    with open(file_path, 'r') as file:
        print('read',file_path)
        lines = file.readlines()

        # Parse border width and height
        original_border_width, original_border_height = map(int, lines[0].split())

        # Calculate the scaling factor
        scaling_factor = 1000 / original_border_width

        # Scale the border width and height
        border_width = 1000
        border_height = int(original_border_height * scaling_factor)

        border_w = border_width
        border_h = border_height

        canvas = 255 * np.zeros((border_height, border_width, 3), dtype=np.uint8)
        border_color = (0, 0, 240) 

        # Parse rectangle coordinates, scale them, and draw on the canvas
        for line in lines[1:]:
            x1, y1, x2, y2 = map(int, line.split())

            # Scale coordinates
            x1_scaled, y1_scaled = int(x1 * scaling_factor), int(y1 * scaling_factor)
            x2_scaled, y2_scaled = int(x2 * scaling_factor), int(y2 * scaling_factor)

            # Extend canvas if box is outside the border
            if x2_scaled > border_width:
                border_width = x2_scaled
                canvas = np.pad(canvas, ((0, 0), (0, x2_scaled - canvas.shape[1]), (0, 0)), mode='constant', constant_values=255)

            if y2_scaled > border_height:
                border_height = y2_scaled
                canvas = np.pad(canvas, ((0, y2_scaled - canvas.shape[0]), (0, 0), (0, 0)), mode='constant', constant_values=255)

            box_border_color = (255, 255, 255)  # Black color for box border
            box_fill_color = (130, 0, 0)  # Red color for box fill
            cv2.rectangle(canvas, (x1_scaled, y1_scaled), (x2_scaled, y2_scaled), box_border_color, 5)  # Draw black rectangles with 4-pixel thickness
            cv2.rectangle(canvas, (x1_scaled, y1_scaled), (x2_scaled, y2_scaled), box_fill_color, -1)  # Fill rectangles with red color

        # Draw green border
        border_thickness = 10
        #cv2.rectangle(canvas, (0, 0), (border_width, border_height), border_color, border_thickness)
        cv2.rectangle(canvas, (0, 0), (border_w, border_h), border_color, border_thickness)


        # Save the image with the original file name
        output_path = os.path.join(output_folder_path, file_name) + ".jpg"
        cv2.imwrite(output_path, canvas)

        print(f"Image saved to {output_path}")


folder_path = f'../loss/{folder_name}'
output_folder_path = f'../plot/{folder_name}'


# Read loss values from the file
file_path = f'../loss/{folder_name}/loss'
with open(file_path, 'r') as file:
    loss_history = [float(line.strip()) for line in file.readlines() if line.strip()]  # Convert to float and ignore empty lines

# Plot the loss history
plt.figure(figsize=(10, 6))
plt.plot(loss_history, color='b', label='Loss')
plt.title('Loss History')
plt.xlabel('Epoch')
plt.ylabel('Loss Value')
plt.grid(True)
plt.legend()
#plt.show()

# Save the plot to an image file
output_file_path = f'../plot/{folder_name}/loss.png'
plt.savefig(output_file_path)

# Display a message indicating the file has been saved
print(f"Plot saved to {output_file_path}")