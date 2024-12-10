import matplotlib.pyplot as plt

# Define your datasets: each dataset is a tuple of two arrays and a distinguishing label 'd'
datasets = [
    {
        "keys": [10, 100, 1000, 10000],
        "total_io": [4, 40, 428,4110],
        "d": "d = 2"
    },
    {
        "keys": [10, 100, 1000, 10000],
        "total_io": [2, 20, 243, 2000],
        "d": "d = 3"
    },

    {
        "keys": [10, 100, 1000, 10000],
        "total_io": [1, 10, 148, 1623],
        "d": "d = 4"

    },

    {
        "keys": [10, 100, 1000, 10000],
        "total_io": [1, 12, 150,1320 ],
        "d": "d = 5"
    }

]

# Plot all datasets
plt.figure(figsize=(10, 6))

for data in datasets:
    plt.plot(data["keys"], data["total_io"], marker='o', linestyle='-', label=data["d"])
    

plt.title("TotalIO vs CurrentNumberOfKeys")
plt.xlabel("CurrentNumberOfKeys")
plt.ylabel("TotalIO")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig('totalio_linear_scale.png')
