import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.animation as animation

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

    # Final snapshot where remaining maybe_bridges are actual bridges
    snapshots.append((not_bridges, maybe_bridges))
    return snapshots

# --- Example Usage ---
num_vertices = 6
edges = [(0,1),(1,2),(2,3),(3,0),(2,4),(4,5)]

snapshots = find_bridges_visual(edges, num_vertices)
G = nx.Graph()
G.add_nodes_from(range(num_vertices))
G.add_edges_from(edges)
pos = nx.spring_layout(G)

fig, ax = plt.subplots(figsize=(6,4))
def update(frame):
    ax.clear()
    not_br, maybe_br = snapshots[frame]
    colors = []
    for e in G.edges():
        if e in not_br or (e[1],e[0]) in not_br:
            colors.append('green')
        elif e in maybe_br or (e[1],e[0]) in maybe_br:
            colors.append('red' if frame == len(snapshots)-1 else 'yellow')
        else:
            colors.append('gray')
    nx.draw(G, pos, ax=ax, with_labels=True, node_color='lightblue', edge_color=colors, width=2)
    ax.set_title(f'Iteration {frame+1}/{len(snapshots)}')

ani = animation.FuncAnimation(fig, update, frames=len(snapshots), interval=1000, repeat=False)
plt.show()

