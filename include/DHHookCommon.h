#import <substrate.h>

/*
 * All hook names are created/specified with $ in place of : for selector names.
 */

/*
 * HOOK(class, name, type, args...)
 *
 * Example:
 * 	HOOK(Class, init, id) {
 * 	  ...
 * 	}
 *
 * 	HOOK(Class, initWithFrame$andOtherThing$, id, CGRect frame, id otherThing) {
 * 	  ...
 * 	}
 *
 * Creates a static variable to store the original message address, and a function to replace it.
 * type is the return type, and args are all the message arguments, in order; args is optional.
 */
#define HOOK(class, name, type, args...) \
	static type (*_ ## class ## $ ## name)(class *self, SEL sel, ## args); \
	static type $ ## class ## $ ## name(class *self, SEL sel, ## args)

/*
 * CALL_ORIG(class, name, args...)
 *
 * Example:
 * 	CALL_ORIG(Class, init);
 * 	CALL_ORIG(Class, initWithFrame$andOtherThing, frame, otherThing);
 *
 * Calls an original implementation.
 */
#define CALL_ORIG(class, name, args...) \
	_ ## class ## $ ## name(self, sel, ## args)

/*
 * GET_CLASS(class)
 *
 * Example:
 * 	GET_CLASS(UIToolbarButton);
 *
 * Simply creates a variable (named $class) to store a class for the HOOK_MESSAGE_* macros.
 * To avoid having to call objc_getClass over and over, and to provide a uniform naming scheme.
 */
#define GET_CLASS(class) \
	Class $ ## class = objc_getClass(#class)

/*
 * HOOK_MESSAGE(class, sel)
 *
 * Example:
 * 	HOOK_MESSAGE(Class, init);
 *
 * Saves the original implementation of sel to a static variable named after _class$sel (created with HOOK), after
 * replacing it with $class$sel (created with HOOK).
 *
 * This exists because sometimes you just want to hook a message with no args, without having to specify a replacement
 * or call __getsel
 */
#define HOOK_MESSAGE(class, sel) \
	_ ## class ## $ ## sel = MSHookMessage($ ## class, @selector(sel), &$ ## class ## $ ## sel) 

static inline SEL __getsel(const char *in) {
	int len = strlen(in) + 1;
	char selector[len];
	for(int i = 0; i < len; i++)
		selector[i] = (in[i] == '$' ? ':' : in[i]);
	return sel_getUid(selector);
}
/*
 * HOOK_MESSAGE_AUTO(class, replace)
 *
 * Example:
 * 	HOOK_MESSAGE_AUTO(Class, initWithFrame$andOtherThing$andAnotherThing$);
 *
 * Beware, __getsel (string copy/transform) is called every time this macro is used.
 * Automatically turns a replacement selector in the $ format into a SEL, as a shorter form of HOOK_MESSAGE_REPLACEMENT.
 *
 * Saves the original implementation to a static variable named after _class$replace (created with HOOK), after
 * replacing it with $class$replace (created with HOOK).
 */
#define HOOK_MESSAGE_AUTO(class, replace) \
	_ ## class ## $ ## replace = MSHookMessage($ ## class, __getsel(#replace), &$ ## class ## $ ## replace)

/*
 * HOOK_MESSAGE_REPLACEMENT(class, sel, replace)
 *
 * Example:
 * 	HOOK_MESSAGE_REPLACEMENT(Class, initWithFrame:andOtherThing:andAnotherThing:, initWithFrame$andOtherThing$andAnotherThing$);
 *
 * Saves the original implementation to a static variable named after _class$replace (created with HOOK), after
 * replacing it with $class$replace (created with HOOK).
 */
#define HOOK_MESSAGE_REPLACEMENT(class, sel, replace) \
	_ ## class ## $ ## replace = MSHookMessage($ ## class, @selector(sel), &$ ## class ## $ ## replace)

#define HOOK_MESSAGE_ARGS HOOK_MESSAGE_WITH_SINGLE_ARG
#define HOOK_MESSAGE_EX HOOK_MESSAGE_AUTO
#define HOOK_MESSAGE_F HOOK_MESSAGE_REPLACEMENT

#define DHGetClass GET_CLASS
#define DHHookMessageWithReplacement HOOK_MESSAGE_REPLACEMENT
#define DHHookMessageWithAutoRename HOOK_MESSAGE_AUTO
