#ifndef PYTHON_VISUALIZER_H
#define PYTHON_VISUALIZER_H

#include <fstream>
#include <string>
#include <vector>

using namespace std;

class PythonVisualizer {
private:
    static void launch(const string& file) {
        string cmd = "start /B python " + file;
        system(cmd.c_str());
        Sleep(Config::SCRIPT_LAUNCH_DELAY_MS);
    }

    static void writeNodes(ofstream& f, const vector<Node>& nodes) {
        f << "nodes_x = [";
        for (size_t i = 0; i < nodes.size(); i++) {
            f << nodes[i].x;
            if (i < nodes.size() - 1) f << ", ";
        }
        f << "]\nnodes_y = [";
        for (size_t i = 0; i < nodes.size(); i++) {
            f << nodes[i].y;
            if (i < nodes.size() - 1) f << ", ";
        }
        f << "]\n\n";
    }

    static void writeEdges(ofstream& f, const string& name,
        const vector<vector<bool>>& matrix, int n) {
        f << name << " = [";
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (matrix[i][j]) f << "(" << i << "," << j << "),";
            }
        }
        f << "]\n\n";
    }

    static void writeEdgePairs(ofstream& f, const string& name,
        const vector<pair<int, int>>& edges) {
        f << name << " = [";
        for (const auto& e : edges) {
            f << "(" << e.first << "," << e.second << "),";
        }
        f << "]\n\n";
    }

    static void writePath(ofstream& f, const string& name, const vector<int>& path) {
        f << name << " = [";
        for (size_t i = 0; i < path.size(); i++) {
            f << path[i];
            if (i < path.size() - 1) f << ",";
        }
        f << "]\n\n";
    }

    static void writePlotSetup(ofstream& f, const string& title, double gridSize, int w, int h) {
        f << "fig, ax = plt.subplots(figsize=(" << w << "," << h << "))\n"
            << "plt.style.use('dark_background')\n"
            << "ax.set_xlim(0," << gridSize << ")\n"
            << "ax.set_ylim(0," << gridSize << ")\n"
            << "ax.set_facecolor('black')\n"
            << "fig.patch.set_facecolor('#0a0a0a')\n"
            << "ax.axis('off')\n"
            << "ax.set_title(\"" << title << "\", color='white', fontsize=18)\n\n";
    }

public:
    static void generateConnectionAnimation(const Network& net, const string& file, const string& title) {
        ofstream f(file);
        if (!f.is_open()) return;

        f << "import matplotlib.pyplot as plt\n"
            << "from matplotlib.animation import FuncAnimation\n\n";

        writeNodes(f, net.getNodes());
        writeEdges(f, "edges", net.getAdjMatrix(), net.getNodeCount());
        writePlotSetup(f, title, net.getGridSize(), 8, 8);

        f << "ax. scatter(nodes_x, nodes_y, c='red', s=12, zorder=5)\n"
            << "lines = []\n"
            << "idx = [0]\n\n"
            << "def init(): return lines\n\n"
            << "def animate(frame):\n"
            << "    if idx[0] < len(edges):\n"
            << "        i, j = edges[idx[0]]\n"
            << "        line, = ax.plot([nodes_x[i], nodes_x[j]], [nodes_y[i], nodes_y[j]],\n"
            << "                        color='#cfcfcf', alpha=0.35, linewidth=0.6)\n"
            << "        lines.append(line)\n"
            << "        idx[0] += 1\n"
            << "    return lines\n\n"
            << "anim = FuncAnimation(fig, animate, init_func=init, frames=len(edges)+1,\n"
            << "                     interval=1, blit=True, repeat=False)\n"
            << "plt. show()\n";

        f.close();
        launch(file);
    }

    static void generateOptimizationAnimation(Network& net, const string& file,
        const string& title,
        const vector<vector<bool>>& before) {
        vector<pair<int, int>> removed, added, kept;
        net.getEdgeChanges(before, removed, added, kept);

        ofstream f(file);
        if (!f.is_open()) return;

        f << "import matplotlib. pyplot as plt\n"
            << "from matplotlib.animation import FuncAnimation\n\n";

        writeNodes(f, net.getNodes());
        writeEdgePairs(f, "to_remove", removed);
        writeEdgePairs(f, "to_add", added);
        writeEdgePairs(f, "staying", kept);
        writePlotSetup(f, title, net.getGridSize(), 8, 8);

        f << "ax. scatter(nodes_x, nodes_y, c='red', s=12)\n"
            << "for i, j in staying:\n"
            << "    ax. plot([nodes_x[i],nodes_x[j]], [nodes_y[i],nodes_y[j]], color='#cfcfcf', alpha=0.35, linewidth=0.6)\n\n"
            << "rem_lines = {}\n"
            << "add_lines = []\n"
            << "for i, j in to_remove:\n"
            << "    line, = ax. plot([nodes_x[i],nodes_x[j]], [nodes_y[i],nodes_y[j]], color='red', alpha=0.6, linewidth=0.8)\n"
            << "    rem_lines[(i,j)] = line\n\n"
            << "def init(): return list(rem_lines. values()) + add_lines\n\n"
            << "def animate(frame):\n"
            << "    rm = len(to_remove)\n"
            << "    ad = len(to_add)\n"
            << "    if frame < rm:\n"
            << "        e = to_remove[frame]\n"
            << "        if e in rem_lines:\n"
            << "            rem_lines[e]. remove()\n"
            << "            del rem_lines[e]\n"
            << "    elif frame < rm + ad:\n"
            << "        i, j = to_add[frame - rm]\n"
            << "        line, = ax.plot([nodes_x[i],nodes_x[j]], [nodes_y[i],nodes_y[j]], color='#00ff00', alpha=0.6, linewidth=0.8)\n"
            << "        add_lines.append(line)\n"
            << "    elif frame == rm + ad:\n"
            << "        for line in add_lines:\n"
            << "            line.set_color('#cfcfcf')\n"
            << "            line.set_alpha(0.35)\n"
            << "            line.set_linewidth(0.6)\n"
            << "    return list(rem_lines.values()) + add_lines\n\n"
            << "anim = FuncAnimation(fig, animate, init_func=init, frames=len(to_remove)+len(to_add)+2,\n"
            << "                     interval=1, blit=True, repeat=False)\n"
            << "plt. show()\n";

        f.close();
        launch(file);
    }

    static void generateComparisonVisualization(const Network& net, const string& file,
        const string& title,
        const vector<vector<bool>>& before,
        const vector<vector<bool>>& after,
        const vector<int>& pathBefore,
        const vector<int>& pathAfter) {
        ofstream f(file);
        if (!f.is_open()) return;

        f << "import matplotlib.pyplot as plt\n\n";

        writeNodes(f, net.getNodes());
        writeEdges(f, "edges_before", before, net.getNodeCount());
        writeEdges(f, "edges_after", after, net.getNodeCount());
        writePath(f, "path_before", pathBefore);
        writePath(f, "path_after", pathAfter);

        double gs = net.getGridSize();

        f << "fig, axes = plt.subplots(1, 2, figsize=(14, 6))\n"
            << "fig.patch.set_facecolor('#111111')\n"
            << "axes[0].set_title('Before:  " << title << "', color='white')\n"
            << "axes[1]. set_title('After: " << title << "', color='white')\n"
            << "for ax in axes:\n"
            << "    ax.set_facecolor('black')\n"
            << "    ax. set_xlim(0," << gs << ")\n"
            << "    ax. set_ylim(0," << gs << ")\n"
            << "    ax.axis('off')\n\n"
            << "for i, j in edges_before:\n"
            << "    axes[0].plot([nodes_x[i],nodes_x[j]], [nodes_y[i],nodes_y[j]], color='#555555', linewidth=0.5, alpha=0.35)\n"
            << "for i, j in edges_after:\n"
            << "    axes[1].plot([nodes_x[i],nodes_x[j]], [nodes_y[i],nodes_y[j]], color='#555555', linewidth=0.5, alpha=0.35)\n\n"
            << "axes[0].scatter(nodes_x, nodes_y, c='red', s=10, zorder=5)\n"
            << "axes[1].scatter(nodes_x, nodes_y, c='red', s=10, zorder=5)\n\n"
            << "def draw_path(ax, path, color):\n"
            << "    if not path:  return\n"
            << "    xs = [nodes_x[i] for i in path]\n"
            << "    ys = [nodes_y[i] for i in path]\n"
            << "    ax.plot(xs, ys, color=color, linewidth=2.2, zorder=10)\n"
            << "    ax. scatter(xs, ys, color=color, s=30, zorder=11)\n\n"
            << "draw_path(axes[0], path_before, 'yellow')\n"
            << "draw_path(axes[1], path_after, 'cyan')\n\n"
            << "plt.tight_layout()\n"
            << "plt.show()\n";

        f.close();
        launch(file);
    }
};

#endif