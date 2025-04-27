import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.widgets import Button
import sys
import random
class UnionFind:
    def __init__(self, n):
        self.parent = list(range(n))
        self.rank = [0]*n

    def find(self, x):
        if self.parent[x] != x:
            self.parent[x] = self.find(self.parent[x])
        return self.parent[x]

    def merge(self, x, y):
        rx, ry = self.find(x), self.find(y)
        if rx == ry:
            return False
        if self.rank[rx] < self.rank[ry]:
            rx, ry = ry, rx
        self.parent[ry] = rx
        if self.rank[rx] == self.rank[ry]:
            self.rank[rx] += 1
        return True

def find_bridges_visual(edges, num_vertices):
    not_bridges = []
    maybe_bridges = list(edges)
    snapshots = []
    snapshots.append((list(not_bridges), list(maybe_bridges)))
    print("snapshots 0:")
    print(snapshots[0])
    while True:
        uf = UnionFind(num_vertices)
        for u, v in not_bridges:
            uf.merge(u, v)

        new_maybe = []
        for u, v in maybe_bridges:
            if uf.merge(u, v):
                new_maybe.append((u, v))
            else:
                not_bridges.append((u, v))

        snapshots.append((list(not_bridges), list(maybe_bridges)))
        if len(new_maybe) == len(maybe_bridges):
            break
        maybe_bridges = new_maybe

    snapshots.append((not_bridges, maybe_bridges))
    return snapshots

def generate_edge_list(num_vertices, avg_degree):
    num_edges = num_vertices * avg_degree // 2
    edge_list = []
    for i in range(0, num_edges):
        vertex_one = random.randint(0, num_vertices - 1)
        vertex_two = random.randint(0, num_vertices -1)
        while(vertex_one == vertex_two):
            vertex_two = random.randint(0, num_vertices -1)
        
        vertex_a = min(vertex_one, vertex_two)
        vertex_b = max(vertex_one, vertex_two)
        edge = (vertex_a, vertex_b)
        if(not (edge in edge_list)):
            edge_list.append(edge)
    return edge_list


def main():
    num_vertices = int(sys.argv[1])
    avg_degree   = int(sys.argv[2])
    edges        = generate_edge_list(num_vertices, avg_degree)
    snapshots    = find_bridges_visual(edges, num_vertices)

    G = nx.Graph()
    G.add_nodes_from(range(num_vertices))
    G.add_edges_from(edges)
    pos = nx.spring_layout(G)

    frame_idx = 0

    fig, ax = plt.subplots(figsize=(6,4))
    plt.subplots_adjust(bottom=0.2)

    def update_plot():
        ax.clear()
        print(snapshots[frame_idx])
        not_br, maybe_br = snapshots[frame_idx]
        print(frame_idx)
        print(not_br)
        colors = []
        for u, v in G.edges():
            if ((u,v) in not_br) or ((v,u) in not_br):
                colors.append('black')
            elif (u,v) in maybe_br or (v,u) in maybe_br:
                c = 'red' if frame_idx == len(snapshots)-1 else 'yellow'
                colors.append(c)
            else:
                colors.append('gray')
        nx.draw(G, pos, ax=ax,
                with_labels=True,
                node_color='lightblue',
                edge_color=colors,
                width=2)
        ax.set_title(f'Iteration {frame_idx+1} / {len(snapshots)}')
        fig.canvas.draw_idle()

    # Button callback
    def on_next(event):
        nonlocal frame_idx
        if frame_idx < len(snapshots)-1:
            frame_idx += 1
            update_plot()
    
    def on_back(event):
        nonlocal frame_idx
        if frame_idx > 0:
            frame_idx -= 1
            update_plot()

    # Create “Next” button
    axnext = plt.axes([0.8, 0.05, 0.1, 0.075])  # x, y, width, height
    axback = plt.axes([0.6, .05, .1, 0.075])
    bnext = Button(axnext, 'Next')
    bback = Button(axback, "Back")
    bnext.on_clicked(on_next)
    bback.on_clicked(on_back)
    
    # Draw the initial frame
    update_plot()

    plt.show()


if __name__ == "__main__":
    main()