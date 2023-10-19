#ifndef __KERNEL_PRINTK__
#define __KERNEL_PRINTK__

/* The log output macros print output to the console. These macros produce
 * compiled log output only if the LOG_LEVEL defined in the makefile (or the
 * make command line) is greater or equal than the level required for that
 * type of log output.
 * The format expected is the same as for INFO(). For example:
 * INFO("Info %s.\n", "message")    -> INFO:    Info message.
 * WARN("Warning %s.\n", "message") -> WARNING: Warning message.
 */

#define KERN_ALERT
#define KERN_CRIT
#define KERN_ERR
#define KERN_WARNING
#define KERN_NOTICE
#define KERN_INFO
#define KERN_DEBUG

#define KERN_DEFAULT

/* integer equivalents of KERN_<LEVEL> */
#define LOGLEVEL_SCHED                                                         \
	-2 /* Deferred messages from sched code are set to this special level */
#define LOGLEVEL_DEFAULT -1 /* default (or last) loglevel */
#define LOGLEVEL_EMERG 0 /* system is unusable */
#define LOGLEVEL_ALERT 1 /* action must be taken immediately */
#define LOGLEVEL_CRIT 2 /* critical conditions */
#define LOGLEVEL_ERR 3 /* error conditions */
#define LOGLEVEL_WARNING 4 /* warning conditions */
#define LOGLEVEL_NOTICE 5 /* normal but significant condition */
#define LOGLEVEL_INFO 6 /* informational */
#define LOGLEVEL_DEBUG 7 /* debug-level messages */

/* Set default LOG_LEVE to warning */
#define LOG_LEVEL LOGLEVEL_WARNING

#define printk printf

#if LOG_LEVEL >= LOGLEVEL_EMERG
#define pr_emerg(...) printf("ERERG:  " __VA_ARGS__)
#else
#define pr_emerg(...)
#endif

#if LOG_LEVEL >= LOGLEVEL_ALERT
#define pr_alert(...) printf("ALERT:  " __VA_ARGS__)
#else
#define pr_alert(...)
#endif

#if LOG_LEVEL >= LOGLEVEL_CRIT
#define pr_crit(...) printf("CRIT:  " __VA_ARGS__)
#else
#define pr_crit(...)
#endif

#if LOG_LEVEL >= LOGLEVEL_ERR
#define pr_err(...) printf("ERR:  " __VA_ARGS__)
#else
#define pr_err(...)
#endif

#if LOG_LEVEL >= LOGLEVEL_WARNING
#define pr_warn(...) printf("WARNING:  " __VA_ARGS__)
#else
#define pr_warn(...)
#endif

#if LOG_LEVEL >= LOGLEVEL_NOTICE
#define pr_notice(...) printf("NOTICE:  " __VA_ARGS__)
#else
#define pr_notice(...)
#endif

#if LOG_LEVEL >= LOGLEVEL_INFO
#define pr_info(...) printf("INFO:  " __VA_ARGS__)
#else
#define pr_info(...)
#endif

/*
 * Like KERN_CONT, pr_cont() should only be used when continuing
 * a line with no newline ('\n') enclosed. Otherwise it defaults
 * back to KERN_DEFAULT.
 */
#define pr_cont(fmt, args...) printf(fmt, ##args)

#if defined(DEBUG) || defined(__DEBUG__) || LOG_LEVEL >= LOGLEVEL_DEBUG
#define pr_debug(fmt, args...) printf(fmt, ##args)
#else
#define pr_debug(fmt, ...)
#endif

#define LOG_LEVEL_NONE			0
#define LOG_LEVEL_ERROR			10
#define LOG_LEVEL_NOTICE		20
#define LOG_LEVEL_WARNING		30
#define LOG_LEVEL_INFO			40
#define LOG_LEVEL_VERBOSE		50

#define LOG_LEVEL 0

#if LOG_LEVEL >= LOG_LEVEL_NOTICE
# define NOTICE(...)	printf("NOTICE:  " __VA_ARGS__)
#else
# define NOTICE(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
# define ERROR(...)	printf("ERROR:   " __VA_ARGS__)
#else
# define ERROR(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARNING
# define WARN(...)	printf("WARNING: " __VA_ARGS__)
#else
# define WARN(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
# define INFO(...)	printf("INFO:    " __VA_ARGS__)
#else
# define INFO(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_VERBOSE
# define VERBOSE(...)	printf("VERBOSE: " __VA_ARGS__)
#else
# define VERBOSE(...)

#endif

#define dev_dbg(dev, ...)	VERBOSE(__VA_ARGS__)
#define dev_err(dev, ...)	ERROR(__VA_ARGS__)
#define dev_info(dev, ...)	INFO(__VA_ARGS__)

#endif /* __KERNEL_PRINTK__ */
