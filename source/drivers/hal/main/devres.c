/**
	所谓的devres,兼容现有的驱动需要这个东西
*/
#include <ddk/types.h>
#include <kernel/ke_lock.h>
#include <ddk/compatible.h>
#include <ddk/slab.h>
#include <ddk/debug.h>

#include <errno.h>

struct devres_node {
	struct list_head		entry;
	dr_release_t			release;
#ifdef CONFIG_DEBUG_DEVRES
	const char			*name;
	size_t				size;
#endif
};

struct devres {
	struct devres_node		node;
	/* -- 3 pointers */
	unsigned long long		data[];	/* guarantee ull alignment */
};

struct devres_group {
	struct devres_node		node[2];
	void				*id;
	int				color;
	/* -- 8 pointers */
};

#define set_node_dbginfo(node, n, s)	do {} while (0)
#define devres_log(dev, node, op)	do {} while (0)

/*
 * Release functions for devres group.  These callbacks are used only
 * for identification.
 */
static void group_open_release(struct device *dev, void *res)
{
	/* noop */
}

static void group_close_release(struct device *dev, void *res)
{
	/* noop */
}

static struct devres_group * node_to_group(struct devres_node *node)
{
	if (node->release == &group_open_release)
		return container_of(node, struct devres_group, node[0]);
	if (node->release == &group_close_release)
		return container_of(node, struct devres_group, node[1]);
	return NULL;
}

static __always_inline struct devres * alloc_dr(dr_release_t release,
						size_t size, gfp_t gfp)
{
	size_t tot_size = sizeof(struct devres) + size;
	struct devres *dr;

	dr = kmalloc_track_caller(tot_size, gfp);
	if (unlikely(!dr))
		return NULL;

	memset(dr, 0, tot_size);
	INIT_LIST_HEAD(&dr->node.entry);
	dr->node.release = release;
	return dr;
}

static void add_dr(struct device *dev, struct devres_node *node)
{
	devres_log(dev, node, "ADD");
	BUG_ON(!list_empty(&node->entry));
	list_add_tail(&node->entry, &dev->devres_head);
}

void * devres_alloc(dr_release_t release, size_t size, gfp_t gfp)
{
	struct devres *dr;

	dr = alloc_dr(release, size, gfp);
	if (unlikely(!dr))
		return NULL;
	return dr->data;
}

void devres_for_each_res(struct device *dev, dr_release_t release,
			dr_match_t match, void *match_data,
			void (*fn)(struct device *, void *, void *),
			void *data)
{
	struct devres_node *node;
	struct devres_node *tmp;
	unsigned long flags;

	if (!fn)
		return;

	spin_lock_irqsave(&dev->devres_lock, flags);
	list_for_each_entry_safe_reverse(node, tmp,
			&dev->devres_head, entry) {
		struct devres *dr = container_of(node, struct devres, node);

		if (node->release != release)
			continue;
		if (match && !match(dev, dr->data, match_data))
			continue;
		fn(dev, dr->data, data);
	}
	spin_unlock_irqrestore(&dev->devres_lock, flags);
}

void devres_free(void *res)
{
	if (res) {
		struct devres *dr = container_of(res, struct devres, data);

		BUG_ON(!list_empty(&dr->node.entry));
		kfree(dr);
	}
}

void devres_add(struct device *dev, void *res)
{
	struct devres *dr = container_of(res, struct devres, data);
	unsigned long flags;

	spin_lock_irqsave(&dev->devres_lock, flags);
	add_dr(dev, &dr->node);
	spin_unlock_irqrestore(&dev->devres_lock, flags);
}

static struct devres *find_dr(struct device *dev, dr_release_t release,
			      dr_match_t match, void *match_data)
{
	struct devres_node *node;

	list_for_each_entry_reverse(node, &dev->devres_head, entry) {
		struct devres *dr = container_of(node, struct devres, node);

		if (node->release != release)
			continue;
		if (match && !match(dev, dr->data, match_data))
			continue;
		return dr;
	}

	return NULL;
}

void * devres_find(struct device *dev, dr_release_t release,
		   dr_match_t match, void *match_data)
{
	struct devres *dr;
	unsigned long flags;

	spin_lock_irqsave(&dev->devres_lock, flags);
	dr = find_dr(dev, release, match, match_data);
	spin_unlock_irqrestore(&dev->devres_lock, flags);

	if (dr)
		return dr->data;
	return NULL;
}

void * devres_get(struct device *dev, void *new_res,
		  dr_match_t match, void *match_data)
{
	struct devres *new_dr = container_of(new_res, struct devres, data);
	struct devres *dr;
	unsigned long flags;

	spin_lock_irqsave(&dev->devres_lock, flags);
	dr = find_dr(dev, new_dr->node.release, match, match_data);
	if (!dr) {
		add_dr(dev, &new_dr->node);
		dr = new_dr;
		new_dr = NULL;
	}
	spin_unlock_irqrestore(&dev->devres_lock, flags);
	devres_free(new_dr);

	return dr->data;
}

void * devres_remove(struct device *dev, dr_release_t release,
		     dr_match_t match, void *match_data)
{
	struct devres *dr;
	unsigned long flags;

	spin_lock_irqsave(&dev->devres_lock, flags);
	dr = find_dr(dev, release, match, match_data);
	if (dr) {
		list_del_init(&dr->node.entry);
		devres_log(dev, &dr->node, "REM");
	}
	spin_unlock_irqrestore(&dev->devres_lock, flags);

	if (dr)
		return dr->data;
	return NULL;
}

int devres_destroy(struct device *dev, dr_release_t release,
		   dr_match_t match, void *match_data)
{
	void *res;

	res = devres_remove(dev, release, match, match_data);
	if (unlikely(!res))
		return -ENOENT;

	devres_free(res);
	return 0;
}

int devres_release(struct device *dev, dr_release_t release,
		   dr_match_t match, void *match_data)
{
	void *res;

	res = devres_remove(dev, release, match, match_data);
	if (unlikely(!res))
		return -ENOENT;

	(*release)(dev, res);
	devres_free(res);
	return 0;
}

static int remove_nodes(struct device *dev,
			struct list_head *first, struct list_head *end,
			struct list_head *todo)
{
	int cnt = 0, nr_groups = 0;
	struct list_head *cur;

	/* First pass - move normal devres entries to @todo and clear
	 * devres_group colors.
	 */
	cur = first;
	while (cur != end) {
		struct devres_node *node;
		struct devres_group *grp;

		node = list_entry(cur, struct devres_node, entry);
		cur = cur->next;

		grp = node_to_group(node);
		if (grp) {
			/* clear color of group markers in the first pass */
			grp->color = 0;
			nr_groups++;
		} else {
			/* regular devres entry */
			if (&node->entry == first)
				first = first->next;
			list_move_tail(&node->entry, todo);
			cnt++;
		}
	}

	if (!nr_groups)
		return cnt;

	/* Second pass - Scan groups and color them.  A group gets
	 * color value of two iff the group is wholly contained in
	 * [cur, end).  That is, for a closed group, both opening and
	 * closing markers should be in the range, while just the
	 * opening marker is enough for an open group.
	 */
	cur = first;
	while (cur != end) {
		struct devres_node *node;
		struct devres_group *grp;

		node = list_entry(cur, struct devres_node, entry);
		cur = cur->next;

		grp = node_to_group(node);
		BUG_ON(!grp || list_empty(&grp->node[0].entry));

		grp->color++;
		if (list_empty(&grp->node[1].entry))
			grp->color++;

		BUG_ON(grp->color <= 0 || grp->color > 2);
		if (grp->color == 2) {
			/* No need to update cur or end.  The removed
			 * nodes are always before both.
			 */
			list_move_tail(&grp->node[0].entry, todo);
			list_del_init(&grp->node[1].entry);
		}
	}

	return cnt;
}

static int release_nodes(struct device *dev, struct list_head *first,
			 struct list_head *end, unsigned long flags)
	__releases(&dev->devres_lock)
{
	LIST_HEAD(todo);
	int cnt;
	struct devres *dr, *tmp;

	cnt = remove_nodes(dev, first, end, &todo);

	spin_unlock_irqrestore(&dev->devres_lock, flags);

	/* Release.  Note that both devres and devres_group are
	 * handled as devres in the following loop.  This is safe.
	 */
	list_for_each_entry_safe_reverse(dr, tmp, &todo, node.entry) {
		devres_log(dev, &dr->node, "REL");
		dr->node.release(dev, dr->data);
		kfree(dr);
	}

	return cnt;
}

int devres_release_all(struct device *dev)
{
	unsigned long flags;

	/* Looks like an uninitialized device structure */
	if (WARN_ON(dev->devres_head.next == NULL))
		return -ENODEV;
	spin_lock_irqsave(&dev->devres_lock, flags);
	return release_nodes(dev, dev->devres_head.next, &dev->devres_head,
			     flags);
}

void * devres_open_group(struct device *dev, void *id, gfp_t gfp)
{
	struct devres_group *grp;
	unsigned long flags;

	grp = kmalloc(sizeof(*grp), gfp);
	if (unlikely(!grp))
		return NULL;

	grp->node[0].release = &group_open_release;
	grp->node[1].release = &group_close_release;
	INIT_LIST_HEAD(&grp->node[0].entry);
	INIT_LIST_HEAD(&grp->node[1].entry);
	set_node_dbginfo(&grp->node[0], "grp<", 0);
	set_node_dbginfo(&grp->node[1], "grp>", 0);
	grp->id = grp;
	if (id)
		grp->id = id;

	spin_lock_irqsave(&dev->devres_lock, flags);
	add_dr(dev, &grp->node[0]);
	spin_unlock_irqrestore(&dev->devres_lock, flags);
	return grp->id;
}

/* Find devres group with ID @id.  If @id is NULL, look for the latest. */
static struct devres_group * find_group(struct device *dev, void *id)
{
	struct devres_node *node;

	list_for_each_entry_reverse(node, &dev->devres_head, entry) {
		struct devres_group *grp;

		if (node->release != &group_open_release)
			continue;

		grp = container_of(node, struct devres_group, node[0]);

		if (id) {
			if (grp->id == id)
				return grp;
		} else if (list_empty(&grp->node[1].entry))
			return grp;
	}

	return NULL;
}

void devres_close_group(struct device *dev, void *id)
{
	struct devres_group *grp;
	unsigned long flags;

	spin_lock_irqsave(&dev->devres_lock, flags);

	grp = find_group(dev, id);
	if (grp)
		add_dr(dev, &grp->node[1]);
	else
		WARN_ON(1);

	spin_unlock_irqrestore(&dev->devres_lock, flags);
}

void devres_remove_group(struct device *dev, void *id)
{
	struct devres_group *grp;
	unsigned long flags;

	spin_lock_irqsave(&dev->devres_lock, flags);

	grp = find_group(dev, id);
	if (grp) {
		list_del_init(&grp->node[0].entry);
		list_del_init(&grp->node[1].entry);
		devres_log(dev, &grp->node[0], "REM");
	} else
		WARN_ON(1);

	spin_unlock_irqrestore(&dev->devres_lock, flags);

	kfree(grp);
}

int devres_release_group(struct device *dev, void *id)
{
	struct devres_group *grp;
	unsigned long flags;
	int cnt = 0;

	spin_lock_irqsave(&dev->devres_lock, flags);

	grp = find_group(dev, id);
	if (grp) {
		struct list_head *first = &grp->node[0].entry;
		struct list_head *end = &dev->devres_head;

		if (!list_empty(&grp->node[1].entry))
			end = grp->node[1].entry.next;

		cnt = release_nodes(dev, first, end, flags);
	} else {
		WARN_ON(1);
		spin_unlock_irqrestore(&dev->devres_lock, flags);
	}

	return cnt;
}

/*
 * Managed kzalloc/kfree
 */
static void devm_kzalloc_release(struct device *dev, void *res)
{
	/* noop */
}

static int devm_kzalloc_match(struct device *dev, void *res, void *data)
{
	return res == data;
}

void * devm_kzalloc(struct device *dev, size_t size, gfp_t gfp)
{
	struct devres *dr;

	/* use raw alloc_dr for kmalloc caller tracing */
	dr = alloc_dr(devm_kzalloc_release, size, gfp);
	if (unlikely(!dr))
		return NULL;

	set_node_dbginfo(&dr->node, "devm_kzalloc_release", size);
	devres_add(dev, dr->data);
	return dr->data;
}

void devm_kfree(struct device *dev, void *p)
{
	int rc;

	rc = devres_destroy(dev, devm_kzalloc_release, devm_kzalloc_match, p);
	WARN_ON(rc);
}

