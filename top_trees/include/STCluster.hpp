#include <memory>

#ifndef ST_CLUSTER_HPP
#define ST_CLUSTER_HPP

namespace TopTree {
class STCluster;
}

#include "ClusterInterface.hpp"
#include "BaseTreeInternal.hpp"
#include "UserFunctions.hpp"

namespace TopTree {

class STCluster : public ICluster, public std::enable_shared_from_this<STCluster> {
friend class STTopTree;
friend class BaseCluster;
friend class CompressCluster;
friend class RakeCluster;
public:
	virtual std::ostream& ToString(std::ostream& o) const = 0;
protected:
	std::shared_ptr<BaseTree::Internal::Vertex> common_vertex;

	std::shared_ptr<STCluster> parent = NULL;
	std::shared_ptr<STCluster> left_child = NULL;
	std::shared_ptr<STCluster> right_child = NULL;
	std::shared_ptr<STCluster> left_foster = NULL;
	std::shared_ptr<STCluster> right_foster = NULL;

	void set_left_child(std::shared_ptr<STCluster> child);
	void set_right_child(std::shared_ptr<STCluster> child);
	void set_left_foster(std::shared_ptr<STCluster> child);
	void set_right_foster(std::shared_ptr<STCluster> child);

	std::list<std::shared_ptr<STCluster>>::iterator root_clusters_iterator;
	bool is_splitted = true; // Initially clusters are in state that they need do_join method (which is called during construction)
	bool is_deleted = false;

	virtual bool isBase() { return false; }
	virtual bool isCompress() { return false; }
	virtual bool isRake() { return false; }

	virtual void do_join() = 0;
	virtual void do_split(std::vector<std::shared_ptr<STCluster>>* splitted_clusters = NULL) = 0;
	virtual void correct_endpoints() = 0;
	virtual void flip() = 0;
	virtual void normalize_for_splay() = 0;

	virtual bool is_handle_for(std::shared_ptr<BaseTree::Internal::Vertex> v) = 0;
	virtual void unregister() = 0;
	virtual void unlink();

	virtual std::ostream& _short_name(std::ostream& o) const = 0; // Used only for debugging
};
std::ostream& operator<<(std::ostream& o, const STCluster& v);

class BaseCluster : public STCluster {
friend class STTopTree;
public:
	virtual std::ostream& ToString(std::ostream& o) const;
	static std::shared_ptr<BaseCluster> construct(std::shared_ptr<BaseTree::Internal::Edge> edge);
protected:

	std::shared_ptr<BaseTree::Internal::Edge> edge;

	bool handles_registered = false;
	std::list<std::shared_ptr<STCluster>>::iterator boundary_left_handles_iterator;
	std::list<std::shared_ptr<STCluster>>::iterator boundary_right_handles_iterator;

	virtual bool isBase() { return true; }
	virtual void do_join();
	virtual void do_split(std::vector<std::shared_ptr<STCluster>>* splitted_clusters = NULL);
	virtual void correct_endpoints() {};
	virtual void flip();
	virtual void normalize_for_splay();

	virtual bool is_handle_for(std::shared_ptr<BaseTree::Internal::Vertex> v);
	virtual void unregister();
	virtual void unlink();

	virtual std::ostream& _short_name(std::ostream& o) const; // Used only for debugging
};

class RakeCluster : public STCluster {
friend class STTopTree;
public:
	virtual std::ostream& ToString(std::ostream& o) const;
	static std::shared_ptr<RakeCluster> construct(std::shared_ptr<STCluster> rake_from, std::shared_ptr<STCluster> rake_to, bool virtual_cluster = false);
protected:

	virtual bool isRake() { return true; }
	virtual void do_join();
	virtual void do_split(std::vector<std::shared_ptr<STCluster>>* splitted_clusters = NULL);
	virtual void correct_endpoints();
	virtual void flip();
	virtual void normalize_for_splay();

	virtual bool is_handle_for(std::shared_ptr<BaseTree::Internal::Vertex> v);
	virtual void unregister();
	virtual void unlink();

	virtual std::ostream& _short_name(std::ostream& o) const; // Used only for debugging
};

class CompressCluster : public STCluster {
friend class STTopTree;
public:
	virtual std::ostream& ToString(std::ostream& o) const;
	static std::shared_ptr<CompressCluster> construct(std::shared_ptr<STCluster> left, std::shared_ptr<STCluster> right);
protected:

	std::shared_ptr<RakeCluster> left_foster_rake = NULL;
	std::shared_ptr<RakeCluster> right_foster_rake = NULL;

	virtual bool isCompress() { return !rakerized; }
	virtual bool isRake() { return rakerized; }
	virtual void do_join();
	virtual void do_split(std::vector<std::shared_ptr<STCluster>>* splitted_clusters = NULL);
	virtual void correct_endpoints();
	virtual void flip();
	virtual void normalize_for_splay();

	virtual bool is_handle_for(std::shared_ptr<BaseTree::Internal::Vertex> v);
	virtual void unregister();
	virtual void unlink();

	virtual std::ostream& _short_name(std::ostream& o) const; // Used only for debugging

	bool rakerized = false; // modyfied to rake node during hard_expose
};

}

#endif // ST_CLUSTER_HPP
