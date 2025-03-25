import csv
import sys

def convert_txt_to_csv(input_file, output_file):
    edges = set()
    
    with open(input_file, 'r') as fin:
        for line in fin:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            
            parts = line.split()
            if len(parts) < 2:
                continue
            
            try:
                node1 = int(parts[0])
                node2 = int(parts[1])
            except ValueError:
                continue
            
            if node1 < 0 or node2 < 0:
                continue

            if node1 == node2:
                continue

            edge = (min(node1, node2), max(node1, node2))
            edges.add(edge)
    
    with open(output_file, 'w', newline='') as fout:
        writer = csv.writer(fout)
        for edge in sorted(edges):
            writer.writerow(edge)

if __name__ == '__main__':
    input_file = "web-Google.txt"
    output_file = "web-google.csv"
    convert_txt_to_csv(input_file, output_file)
