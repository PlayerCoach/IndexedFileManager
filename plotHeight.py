import matplotlib.pyplot as plt

# Data mapping for record height
record_height_mapping = {
    1: 1,
    5: 2,
    23: 3,
    98: 4,
    440: 5,
    1749: 6,
    7982: 7
}

# Extract keys and values
records = list(record_height_mapping.keys())
heights = list(record_height_mapping.values())

# Create the plot
plt.figure(figsize=(8, 6))
plt.plot(records, heights, marker='o', linestyle='-', color='b')

# Set logarithmic scale
plt.xscale('log')
plt.yscale('log')

# Labels and title
plt.xlabel('Number of Records')
plt.ylabel('Tree Height')
plt.title('Tree Height vs. Number of Records (Logarithmic Scale)')

# Display grid and plot
plt.grid(True)
plt.savefig('height.png')
