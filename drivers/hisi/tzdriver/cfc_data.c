#include <linux/types.h>
#define NR_EVENTS 0
#define NR_STATES 2

#define CFC_AUDIT_EVENT_FLAG_TRIGGER_ALL 1

#define CFC_AUDIT_STATE_FLAG_STOP 1
#define CFC_AUDIT_STATE_FLAG_SUCC 2

struct cfc_audit_event {
	u32 flags;
	u64 address;
} __attribute__((packed));

struct cfc_audit_state {
	u32 flags;
	/* idx of events in global state queue */
	u32 states[NR_EVENTS];
} __attribute__((packed));

struct cfc_audit_graph {
	u32 seqlock;
	u32 nr_events;
	u32 nr_states;
	struct cfc_audit_event events[NR_EVENTS];
	struct cfc_audit_state states[NR_STATES];
} __attribute__((packed));


__attribute__ ((__section__(".cfc.auditrules"))) const struct cfc_audit_graph cfc_audit_graph = {
	.seqlock = 0,
	.nr_events = NR_EVENTS,
	.nr_states = NR_STATES,
	.events = {
	},
	.states = {
		{1, {}},
		{0, {}},
	}
};
