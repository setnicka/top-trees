#include "UserFunctions.hpp"
#include "TopologyCluster.hpp"

namespace TopTree {

void TopologyCluster::set_first_child(std::shared_ptr<TopologyCluster> child) {
	first = child;
	if (child != NULL) child->parent = shared_from_this();
}
void TopologyCluster::set_second_child(std::shared_ptr<TopologyCluster> child) {
	second = child;
	if (child != NULL) child->parent = shared_from_this();
}

void TopologyCluster::do_join() {
	if (!is_splitted) return;

	if (vertex != NULL) return; // it is the basic cluster at vertex level

	// 1. Ensure that childs are joined
	if (first != NULL) first->do_join();
	if (second != NULL) second->do_join();

	// 2. Calculate outer edges
	calculate_outer_edges();

	// 3. Series of Joins itself
	if (edge != NULL && !edge->subvertice_edge) {
		// Firstly join one of nodes with edge
		if (first != NULL && first->cluster_data != NULL) Join(first->cluster_data, edge_cluster_data, combined_edge_cluster_data);
		else combined_edge_cluster_data = edge_cluster_data;

		if (second != NULL && second->cluster_data != NULL) Join(combined_edge_cluster_data, second->cluster_data, cluster_data);
		else cluster_data = edge_cluster_data;
	} else if (edge != NULL) {
		Join(first->cluster_data, second->cluster_data, cluster_data);
	} else cluster_data = (first != NULL) ? first->cluster_data : second->cluster_data;

	is_splitted = false;
}

void TopologyCluster::calculate_outer_edges() {
	if (first == NULL && second == NULL) return;

	outer_edges.clear();

	//if (first == NULL) {
	//	for (auto o: second->outer_edges) outer_edges.push_back(neighbour{o.edge, o.cluster->parent});
	//}

	if (second == NULL) {
		for (auto o: first->outer_edges) outer_edges.push_back(neighbour{o.edge, o.cluster->parent});
	} else {
		// Take only unique edges from both children
		for (auto o: first->outer_edges) {
			bool unique = true;
			for (auto oo: second->outer_edges) if (o.edge == oo.edge) {
				// edge = o.edge; // not neede because the second for does it
				unique = false;
			}
			if (unique) outer_edges.push_back(neighbour{o.edge, o.cluster->parent});
		}
		for (auto o: second->outer_edges) {
			bool unique = true;
			for (auto oo: first->outer_edges) if (o.edge == oo.edge) {
				edge = o.edge;
				unique = false;
			}
			if (unique) outer_edges.push_back(neighbour{o.edge, o.cluster->parent});
		}
	}

}

void TopologyCluster::do_split(std::vector<std::shared_ptr<TopologyCluster>>* splitted_clusters) {
	if (is_splitted) return;

	if (vertex != NULL) return; // it is the basic cluster at vertex level

	// 1. Log that this cluster will be splitted
	if (splitted_clusters != NULL) splitted_clusters->push_back(shared_from_this());

	// 2. Ensure that parent is splitted:
	if (parent != NULL) parent->do_split(splitted_clusters);

	// 3. Series of Splits itself
	if (edge != NULL && !edge->subvertice_edge) {
		if (second != NULL && second->cluster_data != NULL) Split(combined_edge_cluster_data, second->cluster_data, cluster_data);
		else combined_edge_cluster_data = cluster_data;

		if (first != NULL && first->cluster_data != NULL) Split(first->cluster_data, edge_cluster_data, combined_edge_cluster_data);
		else edge_cluster_data = combined_edge_cluster_data;
	} else if (edge != NULL) {
		Split(first->cluster_data, second->cluster_data, cluster_data);
	} else {
		if (first != NULL) first->cluster_data = cluster_data;
		else second->cluster_data = cluster_data;
	}

	is_splitted = true;
}

}