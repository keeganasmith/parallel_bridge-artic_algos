import sys
import matplotlib.pyplot as plt
def graph(x, y, xlabel: str, ylabel: str, title: str, xlog: bool, ylog: bool):
    plt.figure(figsize=(8, 6))
    
    if xlog and ylog:
        plt.loglog(x, y, marker='o')
    elif xlog:
        plt.semilogx(x, y, marker='o')
    elif ylog:
        plt.semilogy(x, y, marker='o')
    else:
        plt.plot(x, y, marker='o')
    
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.grid(True, which="both", ls="--", linewidth=0.5)
    
    
    plt.tight_layout()
    file_name = "./graphs/" + title.replace(" ", "_") + ".png"
    plt.savefig(file_name)
    plt.close()

def seperate_single_multi(results):
    proc = results["proc"]
    i = 0
    while(proc[i] != 192):
        i += 1

    single = {}
    multi = {}
    for key in results:
        single[key] = results[key][0:i + 1]
        multi[key] = results[key][i + 1:]
    return single, multi


def strong_scaling_graphs(results):
    single, multi = seperate_single_multi(results)
    single_proc = single["proc"]
    single_time = single["time"]
    multi_proc = multi["proc"]
    multi_time = multi["time"]
    graph(single_proc, single_time, "# processors", "time (s)", "Strong Scaling Single Node", True, True)
    graph(multi_proc, multi_time, "# processors", "time (s)", "Strong Scaling Multi-Node", False, False)
    

def parse(input_file):
    result = ""
    with open(input_file, "r") as my_file:
        result = my_file.read()
    split_by_line = result.split("\n")
    vertice_list = []
    degree_list = []
    time_list = []
    proc_list = []
    i = 0
    while(i < len(split_by_line)):
        line = split_by_line[i]
        if(line.startswith("total iterations")):
            i += 1
            line = split_by_line[i]
            split_line_by_spaces = line.split(" ")
            file_name = split_line_by_spaces[0]
            file_name = file_name.split(".")
            del file_name[0]
            file_name[0] = file_name[0].split("/")
            file_name[0][-1] = file_name[0][-1].split("_")
            vertices = int(file_name[0][-1][0]);
            degree = int(file_name[0][-1][1]);
            time = float(split_line_by_spaces[2][:-1])
            proc = int(split_line_by_spaces[-1])
            vertice_list.append(vertices)
            degree_list.append(degree)
            time_list.append(time)
            proc_list.append(proc)
        else:
            i += 1
    return {"vert": vertice_list, "proc": proc_list, "time": time_list,
            "degree": degree_list}

def main():
    args = sys.argv
    if(len(args) < 3):
        print("usage: ./", args[0], " <file> <type [s|w|d]>")
    input_file_name = args[1]
    scale_type = args[2]
    result = parse(input_file_name)
    if(scale_type == "s"):
        strong_scaling_graphs(result)
    
if __name__ == "__main__":
    main()
