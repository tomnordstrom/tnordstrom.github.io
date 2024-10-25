import csv
import pandas as pd
import os

# Get the directory of the current script
current_dir = os.path.dirname(os.path.abspath(__file__))

machines = ['c20', 'c23']

def format_csv():
    for i in range(len(machines)):
        machine = machines[i]
        # Use os.path.join to create proper file paths
        input_file = os.path.join(current_dir, 'data/input', f'{machine}.csv')
        df = pd.read_csv(input_file)

        # Remove the last 4 characters from the 'File Name' column
        formatted_file = os.path.join(current_dir, 'data/output', f'{machine}_formatted.csv')
        df['File Name'].str[:-4].to_csv(formatted_file, mode='w', index=False)

        df = pd.read_csv(formatted_file)

        # Find duplicates based on 'File Name' column
        duplicates = df[df.duplicated(subset=['File Name'], keep='first')]

        # Save duplicates to a new CSV file
        duplicates_file = os.path.join(current_dir, 'data/output', f'{machine}_duplicates.csv')
        duplicates.to_csv(duplicates_file, index=False)

        # Mark duplicates in the original DataFrame (except for first duplicate)
        df['Duplicate'] = df.duplicated(subset=['File Name'], keep='first')

        # Save the updated DataFrame with the 'Duplicate' column
        df.to_csv(os.path.join(current_dir, 'data/output', f'{machine}_with_duplicates.csv'), index=False)

        # Create a new DataFrame for non-duplicate entries
        non_duplicates = df[~df['Duplicate']]

        # Save all non-duplicate entries to the CSV file
        non_duplicates['File Name'].to_csv(os.path.join(current_dir, 'data/output', f'{machine}_without_duplicates.csv'), index=False)

        output_txt = os.path.join(current_dir, 'data/output', f'{machine}.txt')
        for i in range(len(non_duplicates)):
            with open(output_txt, 'a') as file:
                if i < 200:
                    file.write(str(non_duplicates.iloc[i]['File Name']) + ' or ')
                elif i == 200:
                    file.write("\n" + str(non_duplicates.iloc[i]['File Name']) + ' or ')
                else:
                    file.write(str(non_duplicates.iloc[i]['File Name']) + ' or ')


if __name__ == "__main__":
    format_csv()
