import sys
def parse(input_file):
    string result = ""
    with open(input_file, "r") as my_file:
        result = my_file.read()
    split_by_line = result.split("\n")
    i = 0
    while(i < len(split_by_line)):
        line = split_by_line[i]
        if(line.startswith("total bridges")):
            i += 1
            line = split_by_line[i]
            split_line_by_spaces = line.split(" ")
            file_name = split_line_by_spaces[0]
            file_name = file_name.split(".")
            file_name[0] = file_name[0].split("/")
            file_name[0][-1] = file_name[0][-1].split("_")
            vertices = int(file_name[0][-1][0]);
            degree = int(file_name[0][-1][1]);
            time = float(line[2][:-1])

def main():
    args = sys.argv
    input_file_name = args[1]
    
if __name__ == "__main__":
    main()
