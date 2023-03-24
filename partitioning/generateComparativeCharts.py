import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import sys
import os

directory = sys.argv[1]
file_list = os.listdir(directory)

if not os.path.exists(directory + '/charts'):
    os.makedirs(directory + '/charts')

data = {}

# Loop through all files in the directory
for file in file_list:
    if not os.path.isfile(directory + "/" + file):
        continue
    
    # Read CSV file into a Pandas DataFrame
    df = pd.read_csv(directory + "/" + file, index_col=0)

    # Add the right-most column to the data dictionary with the metric name as key
    metric = file[:-4]  # Remove '.csv' from file name
    data[metric] = df.iloc[:, -1]

# Create a new DataFrame from the data dictionary
df = pd.DataFrame(data)

# Plot the values in the '32' column of the DataFrame
ax = df.plot(kind='line', marker='o', legend=True)
plt.legend(title="Metric")
ax.set_ylabel('Value')
ax.set_title("Comparison of Metrics")
ax.grid(True)
ax.set_xlabel('Category')
ax.set_ylim(ymin=0)
plt.savefig(directory + "/charts/metrics_comparison.png", bbox_inches='tight')