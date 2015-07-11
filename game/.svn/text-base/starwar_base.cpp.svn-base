#include "starwar_base.h"
#include "season_system.h"

namespace gg
{
	void shortestPath_DIJ(const mGraph& graph, mPathList& path_list, int start_id)
	{
		const unsigned n = graph.getPointNum();
		path_list.clear();
		path_list.insert(path_list.end(), n, mPath());
		vector<int> states(n, 0);
		vector<int> distances(n, mGraph::max);

		int current = start_id;
		states[current] = true;

		int minD = mGraph::max;
		int minP = current;

		for (unsigned i = 0; i < n; ++i)
		{
			if (states[i])
				continue;
			distances[i] = graph.getLine(current, i);
			if (distances[i] != mGraph::max)
			{
				path_list[i].push_back(current);
				path_list[i].push_back(i);
			}
			if (distances[i] < minD)
			{
				minD = distances[i];
				minP = i;
			}
		}

		for (unsigned i = 2; i < n; ++i)
		{
			current = minP;
			states[current] = true;
			minD = mGraph::max;
			minP = current;
			for (unsigned j = 0; j < n; ++j)
			{
				if (states[j])
					continue;
				int temp = graph.getLine(current, j) + distances[current];
				if (temp < distances[j])
				{
					distances[j] = temp;
					path_list[j] = path_list[current];
					path_list[j].push_back(j);
				}
				if (distances[j] < minD)
				{
					minD = distances[j];
					minP = j;
				}
			}
		}
	}
}

