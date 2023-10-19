#include <linux/fs.h>
#include <linux/errno.h>
#include <media/v4l2-dev.h>
#include <media/v4l2-device.h>

#define VIDEO_NUM_DEVICES	256

struct video_device *video_device[VIDEO_NUM_DEVICES];

/*
 * All functions are not finished...
*/
struct video_device *video_devdata(struct file *file)
{
	file = file;
//	return video_device[iminor(file_inode(file))];
	return video_device[0];
}

void video_unregister_device(struct video_device *vdev)
{
	vdev = vdev;
}

int __must_check __video_register_device(struct video_device *vdev, int type,
		int nr, int warn_if_nr_in_use, struct module *owner)
{
	return 0;
}

void video_device_release_empty(struct video_device *vdev)
{

}

/* Priority handling */

static inline bool prio_is_valid(enum v4l2_priority prio)
{
	return prio == V4L2_PRIORITY_BACKGROUND ||
	       prio == V4L2_PRIORITY_INTERACTIVE ||
	       prio == V4L2_PRIORITY_RECORD;
}

void v4l2_prio_init(struct v4l2_prio_state *global)
{
	memset(global, 0, sizeof(*global));
}
EXPORT_SYMBOL(v4l2_prio_init);

int v4l2_prio_change(struct v4l2_prio_state *global, enum v4l2_priority *local,
		     enum v4l2_priority new)
{
	if (!prio_is_valid(new))
		return -EINVAL;
	if (*local == new)
		return 0;

	atomic_inc(&global->prios[new]);
	if (prio_is_valid(*local))
		atomic_dec(&global->prios[*local]);
	*local = new;
	return 0;
}
EXPORT_SYMBOL(v4l2_prio_change);

void v4l2_prio_open(struct v4l2_prio_state *global, enum v4l2_priority *local)
{
	v4l2_prio_change(global, local, V4L2_PRIORITY_DEFAULT);
}
EXPORT_SYMBOL(v4l2_prio_open);

void v4l2_prio_close(struct v4l2_prio_state *global, enum v4l2_priority local)
{
	if (prio_is_valid(local))
		atomic_dec(&global->prios[local]);
}
EXPORT_SYMBOL(v4l2_prio_close);

enum v4l2_priority v4l2_prio_max(struct v4l2_prio_state *global)
{
	if (atomic_read(&global->prios[V4L2_PRIORITY_RECORD]) > 0)
		return V4L2_PRIORITY_RECORD;
	if (atomic_read(&global->prios[V4L2_PRIORITY_INTERACTIVE]) > 0)
		return V4L2_PRIORITY_INTERACTIVE;
	if (atomic_read(&global->prios[V4L2_PRIORITY_BACKGROUND]) > 0)
		return V4L2_PRIORITY_BACKGROUND;
	return V4L2_PRIORITY_UNSET;
}
EXPORT_SYMBOL(v4l2_prio_max);

int v4l2_prio_check(struct v4l2_prio_state *global, enum v4l2_priority local)
{
	return (local < v4l2_prio_max(global)) ? -EBUSY : 0;
}
EXPORT_SYMBOL(v4l2_prio_check);
