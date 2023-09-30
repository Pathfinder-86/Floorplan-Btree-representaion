import matplotlib.pyplot as plt
import matplotlib.patches as patches

# Read data from file
file_path = '../output/ami33/best'  # Update the file path accordingly
with open(file_path, 'r') as file:
    lines = file.readlines()

# Parse border width and height
border_width, border_height = map(int, lines[0].split())

# Parse rectangle coordinates
rectangles = []
for line in lines[1:]:
    x1, y1, x2, y2 = map(int, line.split())
    rectangles.append((x1, y1, x2, y2))

# Create a new figure
fig, ax = plt.subplots()

# Set x and y limits based on the border width and height
ax.set_xlim(0, border_width)
ax.set_ylim(0, border_height)

# Plot rectangles
for rect in rectangles:
    x, y, width, height = rect
    rect_patch = patches.Rectangle((x, y), width - x, height - y, linewidth=1, edgecolor='r', facecolor='none')
    ax.add_patch(rect_patch)

# Display the plot
plt.show()