import csv
import pandas as pd

df = pd.read_csv('data\c20.csv')

# Remove the last 4 characters from the 'File Name' column
df['File Name'].str[:-4].to_csv('data\c20_formatted.csv', mode= 'w', index=False)

df = pd.read_csv('data\c20_formatted.csv')

# Find duplicates based on 'File Name' column
duplicates = df[df.duplicated(subset=['File Name'], keep=False)]

# Save duplicates to a new CSV file
duplicates.to_csv('data\c20_duplicates.csv', index=False)

# Mark duplicates in the original DataFrame (except for first duplicate)
df['Duplicate'] = df.duplicated(subset=['File Name'], keep='first')

# Save the updated DataFrame with the 'Duplicate' column
df.to_csv('data\c20_with_duplicates.csv', index=False)

# Create a new DataFrame for non-duplicate entries
non_duplicates = df[~df['Duplicate']]

# Save all non-duplicate entries to the CSV file
non_duplicates['File Name'].to_csv('data\c20_without_duplicates.csv', index=False)

for i in range(len(non_duplicates)):
    if i < 200:
        with open('data\c20.txt', 'a') as file:
            file.write(str(non_duplicates.iloc[i]['File Name']) + ' or ')  
    elif i == 200:
        with open('data\c20.txt', 'a') as file:
            file.write("\n" + str(non_duplicates.iloc[i]['File Name']) + ' or ')  
    else:
        with open('data\c20.txt', 'a') as file:
            file.write(str(non_duplicates.iloc[i]['File Name']) + ' or ')  

#    if df.iloc[i]['Duplicate'] == False:
#        df.iloc[i]['File Name'].to_csv('data\c20_without_duplicates.csv', mode= 'w', index=False)
#    print(df.iloc[i]['File Name'][:-4])
#    print(' or ')
#    with open('data\c20.txt', 'a') as file:
#        
#        file.write(df.iloc[i]['File Name'][:-4] + ' or ')  
