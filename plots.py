import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

plt.style.use('ggplot')

plt.rcParams["font.family"] = "Helvetica Light"

df = pd.read_csv('cache.csv')

pos = list(range(len(df['L3'])))
width = 0.25

plt.rc('xtick', labelsize=8)

fig1, ax1 = plt.subplots(figsize=(10, 5))

plt.xlabel('System')
plt.ylabel('Proportion of cache misses per instruction')

plt.tick_params(axis='x',
                which='both',
                bottom='off',
                top='off',
                right='off',
                labelbottom='on')

plt.bar(pos,
        df['iCache'],
        width,
        alpha=0.5,
        color='#094BD4',
        label=df['System'][0])

plt.bar([p + width for p in pos],
        df['L1'],
        width,
        alpha=0.5,
        color='#2196F3',
        label=df['System'][1])

plt.bar([p + width for p in pos],
        df['L2'],
        width,
        alpha=0.5,
        color='#03A9F4',
        label=df['System'][1])

plt.bar([p + width * 2 for p in pos],
        df['L3'],
        width,
        alpha=0.5,
        color='#00BCD4',
        label=df['System'][2])

ax1.set_axis_bgcolor('white')
ax1.set_label('Misses per instruction')
ax1.set_title('Cache Misses per System')
ax1.set_xticks([p + 1.5 * width for p in pos])
ax1.set_xticklabels(df['System'])

# Setting the x-axis and y-axis limits
plt.xlim(min(pos) - width, max(pos) + width * 4)

# Adding the legend and showing the plot
legend = plt.legend(['iCache Misses', 'L1 Cache Misses', 'L2 Cache Misses', 'L3 Cache Misses'],
                    loc='upper left')
frame = legend.get_frame()
frame.set_facecolor('1')

plt.savefig('individual.eps', format='eps', dpi=1000)

# Draw combined systems
plt.rc('xtick', labelsize=12)

benchmark = df[['iCache', 'L1', 'L2', 'L3']].iloc[[0]].mean(axis=0)
ecs = df[['iCache', 'L1', 'L2', 'L3']].iloc[[1, 2, 3]].mean(axis=0)
oo = df[['iCache', 'L1', 'L2', 'L3']].iloc[[4, 5, 6]].mean(axis=0)

rawCombined = {'function': ['benchmark', 'ecs', 'oop'],
               'iCache': [benchmark['iCache'], ecs['iCache'], oo['iCache']],
               "L1": [benchmark['L1'], ecs['L1'], oo['L1']],
               'L2': [benchmark['L2'], ecs['L2'], oo['L2']],
               'L3': [benchmark['L3'], ecs['L3'], oo['L3']]
               }

dfCombined = pd.DataFrame(rawCombined, columns=['function', 'iCache', 'L1', 'L2', 'L3'])

pos = list(range(len(dfCombined['L3'])))
width = 0.2

fig2, ax2 = plt.subplots(figsize=(10, 5))

plt.xlabel('Design Type')
plt.ylabel('Misses per instruction')

plt.tick_params(axis='x',
                which='both',
                bottom='off',
                top='off',
                right='off',
                labelbottom='on')

plt.bar(pos,
        dfCombined['iCache'],
        width,
        alpha=0.5,
        color='#2196F3',
        label=dfCombined['function'][0])

plt.bar([p + width for p in pos],
        dfCombined['L1'],
        width,
        alpha=0.5,
        color='#03A9F4',
        label=dfCombined['function'][1])

plt.bar([p + width * 2 for p in pos],
        dfCombined['L2'],
        width,
        alpha=0.5,
        color='#00BCD4',
        label=dfCombined['function'][2])

plt.bar([p + width * 3 for p in pos],
        dfCombined['L3'],
        width,
        alpha=0.5,
        color='#00D49C',
        label=dfCombined['function'][2])

ax2.set_axis_bgcolor('white')
ax2.set_label('Misses per instruction')
ax2.set_title('Cache Misses Per Design Type')
ax2.set_xticks([p + 1.5 * width for p in pos])
ax2.set_xticklabels(dfCombined['function'])

# Setting the x-axis and y-axis limits
plt.xlim(min(pos) - width, max(pos) + width * 4)

# Adding the legend and showing the plot
legend = plt.legend(['iCache Misses', 'L1 Cache Misses', 'L2 Cache Misses', 'L3 Cache Misses'],
                    loc='upper left')
frame = legend.get_frame()
frame.set_facecolor('1')

for x, y in zip(pos, dfCombined['iCache']):
    ax2.text(x + + 0.1, y + 0.0003, '%.3f' % y , ha='center', va='bottom')
for x, y in zip(pos, dfCombined['L1']):
    ax2.text(x + 0.3, y + 0.0003, '%.3f' % y , ha='center', va='bottom')
for x, y in zip(pos, dfCombined['L2']):
    ax2.text(x + 0.5, y + 0.0003, '%.3f' % y , ha='center', va='bottom')
for x, y in zip(pos, dfCombined['L3']):
    ax2.text(x + 0.7, y + 0.0003, '%.3f' % y , ha='center', va='bottom')


plt.savefig('designs.eps', format='eps', dpi=1000)

###############
# Draw totals #
###############
fig3, ax3 = plt.subplots(figsize=(10, 5))
dfCombined['means'] = dfCombined.mean(axis=1)

pos = list(range(len(dfCombined['means'])))
width = 0.5

plt.tick_params(axis='x',
                which='both',
                bottom='off',
                top='off',
                right='off',
                labelbottom='on')

plt.bar(pos,
        dfCombined['means'],
        width,
        alpha=0.5,
        color='#00BCD4',
        label=dfCombined['function'], align='center')

ax3.set_axis_bgcolor('white')
plt.xticks(pos, dfCombined['function'])
plt.ylabel('Misses per instruction')
plt.title('Total Cache Misses Per Type')

for x, y in zip(pos, dfCombined['means']):
    ax3.text(x, y + 0.0003, '%.3f' % y , ha='center', va='bottom')

plt.savefig('combined.eps', format='eps', dpi=1000)

############
# Runtimes #
############
dfRuntime = pd.read_csv('data.csv')
dfRuntime['Experiment'] = dfRuntime['Experiment'].astype('category')
dfRuntime['Experiment'] = dfRuntime['Experiment'].cat.rename_categories(
    ['Benchmark', 'OOP', 'ECS']
)

dfRuntime['Average Frame Time'] = dfRuntime['Average Frame Time'].apply(
    lambda val: val * 1000
)

avs = dfRuntime.groupby(['Experiment'], as_index=False).mean()

fig4, ax4 = plt.subplots(figsize=(10, 5))

pos = np.arange(len(avs['Experiment']))
width = 0.5

plt.tick_params(axis='x',
                which='both',
                bottom='off',
                top='off',
                right='off',
                labelbottom='on')

plt.bar(pos,
        avs['Average Frame Time'],
        width,
        alpha=0.5,
        color='#00BCD4',
        label=avs['Experiment'], align='center')

ax4.set_axis_bgcolor('white')
plt.xticks(pos, avs['Experiment'])
plt.ylabel('Average ms per frame')
plt.title('Total Average Frame-Time per test')

for x, y in zip(pos, avs['Average Frame Time']):
    ax4.text(x, y + 0.0002, '%.1f ms' % y, ha='center', va='bottom')

ax4.set_ylim(10, 20)

plt.savefig('total_averages.eps', format='eps', dpi=1000)

###################
# Draw Histograms #
###################

fig5, ax5 = plt.subplots(figsize=(10, 5))

a = dfRuntime.loc[dfRuntime['Experiment'] == 'Benchmark']
b = dfRuntime.loc[dfRuntime['Experiment'] == 'OOP']
c = dfRuntime.loc[dfRuntime['Experiment'] == 'ECS']

print a['Average Frame Time'].mean(axis=0), a['Average Frame Time'].median(axis=0), a['Average Frame Time'].std(axis=0)
print b['Average Frame Time'].mean(axis=0), b['Average Frame Time'].median(axis=0), b['Average Frame Time'].std(axis=0)
print c['Average Frame Time'].mean(axis=0), c['Average Frame Time'].median(axis=0), c['Average Frame Time'].std(axis=0)

bins = np.linspace(
    dfRuntime['Average Frame Time'].min(axis=0),
    dfRuntime['Average Frame Time'].max(axis=0),
    60
)

ax5.hist(a['Average Frame Time'], bins, alpha=0.5)
ax5.hist(b['Average Frame Time'], bins, alpha=0.5)
ax5.hist(c['Average Frame Time'], bins, alpha=0.5)

ax5.set_axis_bgcolor('white')
ax5.xaxis.set_ticks_position('bottom')
ax5.yaxis.set_ticks_position('left')

plt.ylabel('Frequency')
plt.title('Histogram of Frame-Time per test')

# Adding the legend and showing the plot
legend = plt.legend(['Benchmark', 'OOP', 'ECS'],
                    loc='upper center')
frame = legend.get_frame()
frame.set_facecolor('1')

plt.savefig('hist.eps', format='eps', dpi=1000)

plt.grid()
