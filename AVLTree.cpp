// AVL Tree which maintains duplicate values by augmenting a "count" variable to each node.
// Also augment a "subtreeSize" indicating the size of the subtree rooted at this node,
// so that we can find the position of a value in the sorted array. 
class AVLTree {
public:
	typedef struct Node {
		int val;
		int count;
		struct Node *left;
		struct Node *right;
		struct Node *parent;
		int height;
		int subtreeSize;
		Node(int x, Node *p) : val(x), count(1),
			left(NULL), right(NULL), parent(p),
			height(0), subtreeSize(1) {}
	} Node;

	Node *root;

	AVLTree();
	int Size();
	bool ContainsValue(int val);
	void Insert(int val);
	void Delete(int val);
	int ValueAt(int i);
	void PreorderTraversal();
	void PreorderTraversalHelper(Node *x);
	void InorderTraversal();
	void InorderTraversalHelper(Node *x);
	void UpdateHeight(Node *x);
	void UpdateSubtreeSize(Node* x);
	void UpdateNodeInfo(Node *x);
	void UpdateToRoot(Node *x);
	int BalanceFactor(Node *x);
	Node* RotateLeft(Node *p, Node *x);
	Node* RotateRight(Node *p, Node *x);
	Node* RotateLeftRight(Node *p, Node *x);
	Node* RotateRightLeft(Node *p, Node *x);

};

AVLTree::AVLTree() : root(NULL) {
}
int AVLTree::Size() {
	return (root != NULL) ? root->subtreeSize : 0;
}
bool AVLTree::ContainsValue(int val) {
	Node *x = root;
	while (x != NULL)
	{
		if (x->val == val)     return true;
		else if (val < x->val) x = x->left;
		else /*val > x->val*/  x = x->right;
	}
	return false;
}
void AVLTree::Insert(int val) {
	if (root == NULL) {
		root = new Node(val, NULL);
		return;
	}
	Node *x, *p, *g, *n;

	// Find the node "p" which we should insert a new node "val" as a child.
	p = root;
	x = (val < p->val) ? p->left : p->right;
	while (p->val != val && x != NULL) {
		p = x;
		x = (val < p->val) ? p->left : p->right;
	}
	// If this value already exists in the tree, simply increment the count and update up to root.
	if (p->val == val) {
		p->count++;
		p->subtreeSize++;
		UpdateToRoot(p->parent);
		return;
	}

	// Insert the new value as a leaf.
	if (val < p->val) p->left = new Node(val, p);
	else              p->right = new Node(val, p);
	UpdateNodeInfo(p);

	// Retracing back to the root, and rebalancing as we go up.
	for (x = p, p = p->parent; p != NULL; x = p, p = p->parent) {
		// New nodes are only inserted as a leaf, so inserting at the initial x
		// never makes x unbalanced. So x's height is up-to-date.
		// p, the parent of x, depends on x's height.
		// So the balance factors can be computed.
		int balanceX = BalanceFactor(x);
		int balanceP = BalanceFactor(p);

		g = p->parent;
		if (x == p->right) {
			if (balanceP > 1) {
				if (balanceX < 0) n = RotateRightLeft(p, x);
				else              n = RotateLeft(p, x);
			}
			else {
				UpdateNodeInfo(p);  // rotating will update the heights for x,p,n.
									// but if we didn't rotate, the height(x) for the next iteration
									// won't be up-to-date, so do it here (x := p in the next iteration).
				continue;
			}
		}
		else { // (x == p->left)
			if (balanceP < -1) {
				if (balanceX > 0) n = RotateLeftRight(p, x);
				else              n = RotateRight(p, x);
			}
			else {
				UpdateNodeInfo(p);
				continue;
			}
		}
		// Link "n" and to original grandparent node "g".
		n->parent = g;
		if (g == NULL) {
			root = n;
			break;
		}
		else {
			if (g->left == p) g->left = n;
			else              g->right = n;
			UpdateNodeInfo(g);
			p = n;  // Need to set p for the "for" loop update
		}
	}
}
void AVLTree::Delete(int val) {
	//https://en.wikipedia.org/wiki/Binary_search_tree#Deletion
	Node *x, *p, *g, *n, *d, *e, *f;

	// Find the node "d" with the value "val";
	d = root;
	while (d->val != val) {
		d = (val < d->val) ? d->left : d->right;
	}
	// If there is more than one of this value, simply decrement the count.
	// Otherwise, this node has a count of 0 after decrementing, so it must be deleted from the tree.
	// Don't need to decrement the count if it is going to be deleted.
	if (d->count > 1)
	{
		d->count--;
		d->subtreeSize--;
		UpdateToRoot(d->parent);
		return;
	}
	// If d has no children, simply delete it.
	// If d has only one child, move its child into its place.
	// If d has two children, find its in-order predecessor or successor (next smaller or greater)
	// and replace d with that node. If that node has a child, shift the child into that node's place.
	if (d->height == 0) {
		if (d == root) {
			root = NULL;
			delete d;
			return;
		}
		p = d->parent;
		if (p->left == d) p->left = NULL;
		else              p->right = NULL;
		UpdateNodeInfo(p);
		delete d;

		x = p;
	}
	else if (d->left == NULL || d->right == NULL) {
		e = (d->left != NULL) ? d->left : d->right;
		if (d == root) {
			root = e;
			delete d;
			return;
		}
		p = d->parent;
		e->parent = p;
		if (p->left == d) p->left = e;
		else              p->right = e;
		UpdateNodeInfo(p);
		delete d;

		x = p;
	}
	else {
		// Find the next value smaller than the node-to-be-deleted.
		e = d->left;
		while (e->right != NULL) { e = e->right; }
		// Copy over this val to "d". Delete "e" instead.
		// ALSO copy the count. (forgot this)
		d->val = e->val;
		d->count = e->count;
		f = e->left;
		if (f != NULL) f->parent = e->parent;
		if (e->parent->left == e) e->parent->left = f;
		else                      e->parent->right = f;
		UpdateNodeInfo(e->parent);

		x = e->parent;
		//UpdateHeightToRoot(e->parent); // Because I got confused about updating heights o_o
		//                               // This node could be far down the tree, far from "d".
		delete e;
	}
	// x was set to the node whose children was changed (or deleted). x might be unbalanced.
	// Retrace up the tree and rebalance.
	for (p = x; p != NULL; p = p->parent) {
		g = p->parent;
		if (BalanceFactor(p) > 1) {
			x = p->right;
			if (BalanceFactor(x) < 0) n = RotateRightLeft(p, x);
			else                      n = RotateLeft(p, x);
		}
		else if (BalanceFactor(p) < -1) {
			x = p->left;
			if (BalanceFactor(x) > 0) n = RotateLeftRight(p, x);
			else                      n = RotateRight(p, x);
		}
		else {
			UpdateNodeInfo(p);
			//UpdateHeight(p);  // update height(p) for the next iteration since we didn't 
			//                  // update it in a rotate()
			continue;
		}
		// Link "n" and to original grandparent node "g".
		n->parent = g;
		if (g == NULL) {
			root = n;
			break;
		}
		else {
			if (g->left == p) g->left = n;
			else              g->right = n;
			UpdateHeight(g);
			p = n;  // Need to set p for the "for" loop update
		}
	}
}
int AVLTree::ValueAt(int i) {
	Node *x = root;
	int leftSize = (x->left != NULL) ? x->left->subtreeSize : 0;
	// Stop when leftSize <= i < leftSize + x->count;
	while (x != NULL) {
		if (i < leftSize) {
			x = x->left;
		}
		else if (i >= leftSize + x->count) {
			i -= leftSize + x->count;
			x = x->right;
		}
		else {
			break;
		}
		leftSize = (x->left != NULL) ? x->left->subtreeSize : 0;
	}
	// Before returning, check for null?
	if (x != NULL) {
		return x->val;
	}
	else {
		printf("index out of bounds\n");
		return 0;
	}
}
void AVLTree::PreorderTraversal() {
	PreorderTraversalHelper(root);
}
void AVLTree::PreorderTraversalHelper(Node *x) {
	if (x == NULL) return;
	printf("(x=%d, n=%d, BF=%d, H=%d, size=%d) ", x->val, x->count, BalanceFactor(x), x->height, x->subtreeSize);
	PreorderTraversalHelper(x->left);
	PreorderTraversalHelper(x->right);
}
void AVLTree::InorderTraversal() {
	InorderTraversalHelper(root);
}
void AVLTree::InorderTraversalHelper(Node *x) {
	if (x == NULL) return;
	InorderTraversalHelper(x->left);
	printf("(x=%d, n=%d, BF=%d, H=%d, size=%d) \n", x->val, x->count, BalanceFactor(x), x->height, x->subtreeSize);
	InorderTraversalHelper(x->right);
}
void AVLTree::UpdateHeight(Node* x) {
	int leftHeight = (x->left != NULL) ? x->left->height : -1;
	int rightHeight = (x->right != NULL) ? x->right->height : -1;

	x->height = 1 + ((leftHeight > rightHeight) ? leftHeight : rightHeight);
}
void AVLTree::UpdateSubtreeSize(Node* x) {
	int leftSubtreeSize = (x->left != NULL) ? x->left->subtreeSize : 0;
	int rightSubtreeSize = (x->right != NULL) ? x->right->subtreeSize : 0;

	x->subtreeSize = x->count + leftSubtreeSize + rightSubtreeSize;
}
void AVLTree::UpdateNodeInfo(Node *x) {
	UpdateHeight(x);
	UpdateSubtreeSize(x);
}
void AVLTree::UpdateToRoot(Node *x) {
	while (x != NULL) {
		UpdateNodeInfo(x);
		x = x->parent;
	}
}
int AVLTree::BalanceFactor(Node* x) {
	int leftHeight = (x->left != NULL) ? x->left->height : -1;
	int rightHeight = (x->right != NULL) ? x->right->height : -1;

	return rightHeight - leftHeight;
}
AVLTree::Node* AVLTree::RotateLeft(Node *p, Node *x) {
	p->right = x->left;
	x->left = p;
	UpdateNodeInfo(p);
	UpdateNodeInfo(x);

	if (p->right != NULL) p->right->parent = p;
	p->parent = x;

	return x;
}
AVLTree::Node* AVLTree::RotateRight(Node *p, Node *x) {
	p->left = x->right;
	x->right = p;
	UpdateNodeInfo(p);
	UpdateNodeInfo(x);

	if (p->left != NULL) p->left->parent = p;
	p->parent = x;

	return x;
}
AVLTree::Node* AVLTree::RotateLeftRight(Node *p, Node *x) {
	Node *n = x->right;
	x->right = n->left;
	p->left = n->right;
	n->left = x;
	n->right = p;
	UpdateNodeInfo(x);
	UpdateNodeInfo(p);
	UpdateNodeInfo(n);

	if (x->right != NULL) x->right->parent = x;
	if (p->left != NULL) p->left->parent = p;
	x->parent = n;
	p->parent = n;

	return n;
}
AVLTree::Node* AVLTree::RotateRightLeft(Node *p, Node *x) {
	Node *n = x->left;
	p->right = n->left;
	x->left = n->right;
	n->left = p;
	n->right = x;
	UpdateNodeInfo(p);
	UpdateNodeInfo(x);
	UpdateNodeInfo(n);

	if (p->right != NULL) p->right->parent = p;
	if (x->left != NULL) x->left->parent = x;
	p->parent = n;
	x->parent = n;

	return n;
}
