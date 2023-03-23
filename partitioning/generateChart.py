import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('timing.csv', names=['Category', 1, 2, 4, 8, 16, 32], skiprows=1)

# Generate a bar chart of the data
ax = df.plot(kind='line', x='Category', y=[1, 2, 4, 8, 16, 32], marker='o', legend=True)

# Set the chart title and axes labels
ax.set_title('')
ax.set_xticks(range(1, 19))
ax.set_xticklabels(range(1, 19))
ax.set_xlabel('Hash bits')
ax.set_ylabel('time ms')


plt.savefig('chart.png', bbox_inches='tight')
