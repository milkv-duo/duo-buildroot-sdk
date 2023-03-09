#include <linux/string.h>
#include <match_table.h>

/*****************************************************************************/
int reg2type(struct match_reg_type *table, int length, int reg, int def)
{
	while (length-- > 0) {
		if (table->reg == reg)
			return table->type;
		table++;
	}
	return def;
}

int type2reg(struct match_reg_type *table, int length, int type, int def)
{
	while (length-- > 0) {
		if (table->type == type)
			return table->reg;
		table++;
	}
	return def;
}

int str2type(struct match_type_str *table, int length, const char *str,
	     int size, int def)
{
	while (length-- > 0) {
		if (!strncmp(table->str, str, size))
			return table->type;
		table++;
	}
	return def;
}

const char *type2str(struct match_type_str *table, int length, int type,
		     const char *def)
{
	while (length-- > 0) {
		if (table->type == type)
			return table->str;
		table++;
	}
	return def;
}

int match_reg_to_type(struct match_t *table, int nr_table, int reg, int def)
{
	while (nr_table-- > 0) {
		if (table->reg == reg)
			return table->type;
		table++;
	}
	return def;
}

int match_type_to_reg(struct match_t *table, int nr_table, int type, int def)
{
	while (nr_table-- > 0) {
		if (table->type == type)
			return table->reg;
		table++;
	}
	return def;
}

int match_data_to_type(struct match_t *table, int nr_table, char *data,
		int size, int def)
{
	while (nr_table-- > 0) {
		if (!memcmp(table->data, data, size))
			return table->type;
		table++;
	}
	return def;
}

void *match_type_to_data(struct match_t *table, int nr_table, int type,
			 void *def)
{
	while (nr_table-- > 0) {
		if (table->type == type)
			return table->data;
		table++;
	}
	return def;
}

