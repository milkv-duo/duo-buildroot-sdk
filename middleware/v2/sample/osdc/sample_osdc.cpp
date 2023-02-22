#include <fstream>
#include <sstream>
#include <sys/time.h>
#include <map>
#include "osd_cmpr.h"
#include "cmpr_canvas.h"

#define PRINT_LOG(fmt, ...)                                                    \
	do {                                                                   \
		if (print_log_en)                                              \
			printf(fmt, __VA_ARGS__);                              \
	} while (0)

#define TEST_PERF_ITER (50)

string readCsvFileIntoString(const string &path)
{
	auto ss = ostringstream{};
	ifstream input_file(path);
	if (!input_file.is_open()) {
		printf("Could not open the file %s \n", path.c_str());
		exit(EXIT_FAILURE);
	}
	ss << input_file.rdbuf();
	return ss.str();
}

int analyzeCsvContents(string file_contents,
		       std::map<int, std::vector<string> > &csv_contents)
{
	istringstream sstream(file_contents);
	std::vector<string> items;
	string record;
	int line_cnt = 0;
	while (std::getline(sstream, record)) {
		istringstream line(record);
		while (std::getline(line, record, ',')) {
			items.push_back(record);
		}
		csv_contents[line_cnt] = items;
		items.clear();
		line_cnt++;
	}
	return line_cnt;
}

bool parse_command(int argc, char *argv[], string &file_name, string &in_dir,
		   string &out_dir, int &img_width, int &img_height,
		   OSD_FORMAT &osd_format, bool &wrt_draw, bool &wrt_bs)
{
	string file_key = "-f";
	string in_dir_key = "-in_dir";
	string out_dir_key = "-out_dir";
	string width_key = "-w";
	string height_key = "-h";
	string format_key = "-format";
	string wrt_draw_key = "-wrt_draw";
	string wrt_bs_key = "-wrt_bs";

	for (int arg_i = 0; arg_i < argc; arg_i++) {
		string key = argv[arg_i];
		if (!file_key.compare(key)) {
			file_name = argv[arg_i + 1];
		} else if (!in_dir_key.compare(key)) {
			in_dir = argv[arg_i + 1];
		} else if (!out_dir_key.compare(key)) {
			out_dir = argv[arg_i + 1];
		} else if (!width_key.compare(key)) {
			img_width = atoi(argv[arg_i + 1]);
		} else if (!height_key.compare(key)) {
			img_height = atoi(argv[arg_i + 1]);
		} else if (!format_key.compare(key)) {
			osd_format = (OSD_FORMAT)atoi(argv[arg_i + 1]);
		} else if (!wrt_draw_key.compare(key)) {
			wrt_draw = atoi(argv[arg_i + 1]);
		} else if (!wrt_bs_key.compare(key)) {
			wrt_bs = atoi(argv[arg_i + 1]);
		}
	}
	return true;
}

vector<DRAW_OBJ> parse_obj_from_csv(Canvas_Attr canvas, string csv_dir)
{
	string csv_contents = readCsvFileIntoString(csv_dir);
	std::map<int, std::vector<string> > csv_row_list;
	int num = analyzeCsvContents(csv_contents, csv_row_list) - 1;
	vector<DRAW_OBJ> obj_vec;
	if (num < 0) {
		printf("parse obj csv file fails!\n");
		goto END;
	}
	for (int row_i = 0; row_i < num; row_i++) {
		vector<string> csv_row = csv_row_list[row_i + 1];
		if (stoi(csv_row[0]) <= 0) {
			continue;
		}
		DRAW_OBJ obj;
		DRAW_OBJ_TYPE type = (DRAW_OBJ_TYPE)stoi(csv_row[1]);
		int thickness = stoi(csv_row[2]);
		if (type == RECT || type == STROKE_RECT || type == BIT_MAP ||
		    type == CMPR_BIT_MAP) {
			int pt_x = stoi(csv_row[4]);
			int pt_y = stoi(csv_row[5]);
			int width = stoi(csv_row[6]);
			int height = stoi(csv_row[7]);
			if (type == BIT_MAP || type == CMPR_BIT_MAP) {
				FILE *f_bitmap =
					fopen(csv_row[3].c_str(), "rb");
				uint8_t *buf, *cmpr_buf;
				int bitmap_sz;
				if (f_bitmap) {
					bitmap_sz = width * height *
						    osd_cmpr_get_pixel_sz(
							    canvas.format);
					buf = (uint8_t *)malloc(bitmap_sz);
					fread(buf, bitmap_sz, sizeof(uint8_t),
					      f_bitmap);
					fclose(f_bitmap);
				} else {
					printf("open bitmap file %s fails! \n",
					       csv_row[3].c_str());
					continue;
				}
				bool cmpr_status = false;
				if (type == CMPR_BIT_MAP) {
					cmpr_buf = (uint8_t *)malloc(bitmap_sz);
					uint32_t  cmpr_bitmap_sz;
					cmpr_status =
						cmpr_bitmap(&canvas, buf,
							    cmpr_buf, width,
							    height, bitmap_sz,
							    &cmpr_bitmap_sz);
				}
				set_bitmap_obj_attr(
					&obj, &canvas,
					(cmpr_status) ? cmpr_buf : buf, pt_x,
					pt_y, width, height, cmpr_status);
				if (cmpr_status) {
					free(buf);
				}
			} else { // type==LINE
				uint32_t color_code =
					stoul(csv_row[3], nullptr, 16);
				set_rect_obj_attr(&obj, &canvas, color_code, pt_x,
						  pt_y, width, height,
						  type == RECT, thickness);
			}
		} else { // LINE
			int pt_x[2], pt_y[2];
			for (int side_i = 0; side_i < 2; side_i++) {
				pt_x[side_i] = stoi(csv_row[side_i * 2 + 4]);
				pt_y[side_i] = stoi(csv_row[side_i * 2 + 5]);
			}
			uint32_t color_code = stoul(csv_row[3], nullptr, 16);
			set_line_obj_attr(&obj, &canvas, color_code, pt_x[0],
					  pt_y[0], pt_x[1], pt_y[1], thickness);
		}
		obj_vec.push_back(obj);
	}
END:
	return obj_vec;
}

//#define OBJ_FROM_CSV
int main(int argc, char *argv[])
{
	bool print_log_en = true, wrt_draw = false, wrt_bs = false;
	uint32_t bs_size = 0;
	// parse command line args
	int img_width, img_height;
	OSD_FORMAT osd_format;
	int bs_buf_size;

#ifdef OBJ_FROM_CSV
	string in_dir, out_dir, csv_name;

	parse_command(argc, argv, csv_name, in_dir, out_dir, img_width,
		      img_height, osd_format, wrt_draw, wrt_bs);

	size_t pel_num = img_width * img_height;
	size_t pel_sz = osd_cmpr_get_pixel_sz(osd_format);

	Canvas_Attr canvas = { img_width, img_height, osd_format, BG_COLOR_CODE };

	vector<DRAW_OBJ> obj_vec =
		parse_obj_from_csv(canvas, in_dir + "/" + csv_name);
	if (obj_vec.size() <= 0) {
		printf("no draw obj parsed, stop! \n");
		return 0;
	}

	bs_buf_size = est_cmpr_canvas_size(&canvas, obj_vec.data(), obj_vec.size());
#else
	(void)argc;
	(void)argv;

	osd_format = OSD_ARGB8888;
	img_width = 2560;
	img_height =1440;
	size_t pel_num = img_width * img_height;
	size_t pel_sz = osd_cmpr_get_pixel_sz(osd_format);
	Canvas_Attr canvas = { img_width, img_height, osd_format, BG_COLOR_CODE };
	DRAW_OBJ obj_vec;

	set_rect_obj_attr(&obj_vec, &canvas, 0xffffffff, 100, 100, 200, 200, true, 4);

	bs_buf_size = est_cmpr_canvas_size(&canvas, &obj_vec, 1);
#endif

	uint8_t *bsbuf = (uint8_t *)calloc(bs_buf_size, sizeof(uint8_t));
	int status;

#if (TEST_PERF_ITER)
	struct timeval startTime, endTime;
	gettimeofday(&startTime, NULL);
	for (int iter = 0; iter < TEST_PERF_ITER; iter++) {
#endif

#ifdef OBJ_FROM_CSV
	status = draw_cmpr_canvas(&canvas, obj_vec.data(), obj_vec.size(), bsbuf, bs_buf_size,
					  &bs_size);
#else
	status = draw_cmpr_canvas(&canvas, &obj_vec, 1, bsbuf, bs_buf_size,
					  &bs_size);
#endif

#if (TEST_PERF_ITER)
	}
	// stop timer.
	gettimeofday(&endTime, NULL);
	int64_t proc_time = ((endTime.tv_sec - startTime.tv_sec) * 1e6 +
			     (endTime.tv_usec - startTime.tv_usec));
	PRINT_LOG("draw cmpr canvas time(us), %.3f \n",
		  proc_time / (double)TEST_PERF_ITER);
#endif

	if (status != 1) {
		printf("draw_cmpr_canvas_stream fail, stop! \n");
		return 0;
	}
	// -------------------- Compressed stream verification --------------------

	// cfg default
	OSDCmpr_Cfg osd_cfg;
	osd_cfg.img_width = img_width;
	osd_cfg.img_height = img_height;
	osd_cfg.palette_mode_en = OSDEC_PAL_BD > 0;
	osd_cfg.run_len_bd = OSDEC_RL_BD;
	osd_cfg.palette_idx_bd = OSDEC_PAL_BD;
	osd_cfg.zeroize_by_alpha = false;
	osd_cfg.rgb_trunc_bit = 0;
	osd_cfg.alpha_trunc_bit = 0;
	osd_cfg.osd_format = osd_format;

	// command info
	OSDCmpr_Ctrl osdCmpr_ctrl;
	//memset(&osdCmpr_ctrl, 0, sizeof(OSDCmpr_Ctrl));
	osdCmpr_ctrl = {};

	osd_cmpr_setup(&osdCmpr_ctrl, &osd_cfg);
	int hdr_sz = osd_cmpr_get_header_sz();
	uint8_t *decbuf = (uint8_t *)calloc(pel_num, pel_sz);
	osd_cmpr_frame_init(&osdCmpr_ctrl);
	osd_cmpr_dec_one_frame(&bsbuf[hdr_sz], bs_size - hdr_sz, decbuf,
			       &osdCmpr_ctrl);

#if (CMPR_CANVAS_DBG)
	uint8_t *golden_buf = (uint8_t *)calloc(pel_num, pel_sz);

#if (TEST_PERF_ITER)
	struct timeval startTime1, endTime1;
	gettimeofday(&startTime1, NULL);
	for (int iter = 0; iter < TEST_PERF_ITER; iter++) {
#endif

		//draw_canvas_raw_buffer2(canvas, obj_vec, golden_buf);
		draw_canvas_raw_buffer(canvas, obj_vec, golden_buf);

#if (TEST_PERF_ITER)
	}
	// stop timer.
	gettimeofday(&endTime1, NULL);
	int64_t proc_time1 = ((endTime1.tv_sec - startTime1.tv_sec) * 1e6 +
			      (endTime1.tv_usec - startTime1.tv_usec));
	PRINT_LOG("draw raw buffer time(us), %.3f \n",
		  proc_time1 / (double)TEST_PERF_ITER);
#endif

	uint8_t *debug_buf = (uint8_t *)calloc(bs_buf_size, sizeof(uint8_t));
	osd_cmpr_enc_header(debug_buf, &osdCmpr_ctrl);
	osd_cmpr_frame_init(&osdCmpr_ctrl);
	int bs_size_debug = osd_cmpr_enc_one_frame(
		golden_buf, &debug_buf[hdr_sz], &osdCmpr_ctrl);

	// --- compare Enc/Dec ---
	if (memcmp(bsbuf, debug_buf, bs_size) != 0) {
		printf("golden bitstream mismatch, fuck!!!!!!\n");
	}

	if (memcmp(golden_buf, decbuf, pel_num * pel_sz) != 0) {
		printf("enc/dec data mismatch, fuck!!!!!!\n");
	}
#endif // CMPR_CANVAS_DBG

	if (wrt_bs) {
		string bs_file_name = "sample_osdc_bs.bin";
		FILE *f_out_bs = fopen(bs_file_name.c_str(), "wb");
		fwrite(bsbuf, bs_size, sizeof(uint8_t), f_out_bs);
	}
	if (wrt_draw) {
		string out_file = "sample_osdc_draw.bin";
		FILE *f_out = fopen(out_file.c_str(), "wb");
		fwrite(decbuf, pel_num, pel_sz, f_out);
	}
	// --- summary ---
	PRINT_LOG("%dx%d, cmpr, %d, org, %d,CR, %.3f\n", img_width, img_height,
		  bs_size, (int)(pel_num * pel_sz),
		  bs_size / ((double)pel_num * pel_sz));

	if (bsbuf) {
		free(bsbuf);
	}
	return 0;
}
