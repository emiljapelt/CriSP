import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

df = pd.read_csv('./benchmark_data/server_8passes_concurrent.csv', names=['Category', 1, 2, 4, 8, 16, 32], skiprows=1)

# Generate a bar chart of the data
df.iloc[:, 1:] = 16777216 / (df.iloc[:, 1:]/1000)
ax = df.plot(kind='line', x='Category', y=[1, 2, 4, 8, 16, 32], marker='o', legend=True)
# Set the chart title and axes labels
ax.set_title('')
ax.grid(True)
ax.set_xticks(range(1, 19))
ax.set_xticklabels(range(1, 19))
ax.set_xlabel('Hash bits')

formatter = ticker.FuncFormatter(lambda x, pos: '{:.0f}'.format(round(x/1000000)))
ax.yaxis.set_major_formatter(formatter)
ax.set_ylabel('Million tuples per second')


plt.savefig('chart.png', bbox_inches='tight')
