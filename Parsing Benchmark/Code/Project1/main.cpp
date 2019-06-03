/*
 * @ Author : Jaehwan Jeong
 * @ Date : 2019-5-25
 * @ Brief description : Benchmark Parsing code for Advanced OS Project (collecting I/O trace from Systor'17)
 */

#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <math.h>

typedef unsigned long long u64_t;
#define RUN_SPECIFER (pow(2,50))
// LUN : Storage Name, index goes to 0-4, 6 (no 5)
// 2016-02-22-07 (year: 2016, month : 02, day : 22, time : 07am)

//global variable for sorting

std::multimap<double, std::string> map_for_sort;

void append_each_storage(std::string& file_name, std::ofstream& out)
{
	std::string one_line;

	for (int idx = 0; idx < 7; ++idx)
	{
		if (idx == 5)
			continue;
		std::ifstream each_file(file_name + "-LUN" + std::to_string(idx) + ".csv");

		if (!each_file.is_open())
			std::cerr << ("no " + std::to_string(idx) + " LUN exists in " + file_name + "\n");
		else
		{
			std::getline(each_file, one_line);
			while (!each_file.eof())
			{
				std::getline(each_file, one_line);
				if (one_line == "")
					continue;
				out << one_line << '\n';
			}
			each_file.close();
		}
	}
}

void append_each_sorted_file(std::string & file_name, std::ofstream & out)
{
	std::string one_line;

	std::ifstream input_file(file_name);

	if (!input_file.is_open())
		std::cerr << "not exist file_name\n";
	else
	{
		while (!input_file.eof())
		{
			getline(input_file, one_line);
			if (one_line == "")
				continue;
			else
				out << one_line << '\n';
		}
		
	}
}


void sort_total(std::string file_name)
{
	std::string one_line;
	std::ifstream total_file(file_name + "-Total.csv");
	char* tmp;


	if (!total_file.is_open())
		std::cerr << "doesn't exist total_file_name\n";
	else
	{
		int i = 0;
		while (!total_file.eof())
		{
			i++;
			std::getline(total_file, one_line);
			if (one_line == "")
				continue;
			int idx = one_line.find_first_of(",");


			std::string timestamp_string = one_line.substr(0, idx);
			double timestamp = stod(timestamp_string);
			std::string string_except_timestamp = one_line.substr(idx, one_line.size() - idx);
			map_for_sort.insert(std::make_pair(timestamp, string_except_timestamp));

		}
		total_file.close();
	}
	

	std::ofstream sorted_total_file(file_name + "-Total_sort.csv");

	std::map<double, std::string>::iterator it;
	for (it = map_for_sort.begin(); it != map_for_sort.end(); ++it)
	{
		sorted_total_file << std::to_string((*it).first) + (*it).second << '\n';
	}

	map_for_sort.clear();
	sorted_total_file.close();
	
}

u64_t append_address(int _run_num, u64_t blk_offset)
{
	
	/*u64_t result = 0;
	
	switch (_run_num)
	{
	case 0:
		result = 0 * RUN_SPECIFER + blk_offset;
		break;
	case 1:
		result = 1 * RUN_SPECIFER + blk_offset;
		break;
	case 2:
		result = 2 * RUN_SPECIFER + blk_offset;
		break;
	case 3:
		result = 3 * RUN_SPECIFER + blk_offset;
		break;
	case 4:
		result = 4 * RUN_SPECIFER + blk_offset;
		break;
	case 6:
		result = 5 * RUN_SPECIFER + blk_offset;
		break;
	default:
		break;
	}
	*/
	u64_t result = _run_num * RUN_SPECIFER + blk_offset;

	return result;
}

bool isFormatTrue(std::string str);

void parse_disk_page_IO(std::string file_name)
{
	std::ifstream inputfile(file_name);
	std::ofstream result_file(file_name + "_Final.txt", std::ios::app);
	result_file << "Timestamp,NumberOfPages,BlockOffset\n";


	std::string one_line;


	if (!inputfile.is_open())
		std::cout << "no file exist for parsing\n";

	else
	{
		int i = 0;
		while (!inputfile.eof())
		{
			i++;
			getline(inputfile, one_line);
			if (one_line == "")
				continue;

			if (isFormatTrue(one_line) == false)
				continue;

			int idx_timestamp = one_line.find_first_of(",");
			std::string timestamp = one_line.substr(0, idx_timestamp);

			
			if (one_line.find_last_of(",") == std::string::npos)
				continue;
			uint32_t idx_pgsize = one_line.find_last_of(",");
			
			std::string page_size_str = one_line.substr( idx_pgsize + 1, one_line.size() - idx_pgsize);

			u64_t page_size = std::stoull(page_size_str, nullptr, 0);

			std::string without_page = one_line.substr(0, idx_pgsize);

			if (without_page.find_last_of(",") == std::string::npos)
				continue;
			uint32_t idx_blk_offset = without_page.find_last_of(",");
			std::string blk_offset_str = without_page.substr(idx_blk_offset +1, without_page.size() - idx_blk_offset);

			u64_t blk_offset = std::stoull(blk_offset_str, nullptr, 0);


			std::string without_offset = without_page.substr(0, idx_blk_offset);

			if (without_offset.find_last_of(",") == std::string::npos)
				continue;
			int idx_run_offset = without_offset.find_last_of(",");
			std::string run_offset_str = without_offset.substr(idx_run_offset +1, without_offset.size() - idx_run_offset);

			int run_offset = std::stoi(run_offset_str);

			blk_offset = append_address(run_offset, blk_offset);

			u64_t num_of_pages = ((page_size % 4096) == 0) ? (page_size / 4096) : (page_size / 4096) + 1;

			result_file << timestamp << " " << num_of_pages << " " << blk_offset << '\n';
		}

	}
}


void set_day_vec_for_additional(std::vector<std::string> & _vec)
{
	std::string feb = "02";
	for (int i = 16; i <= 21; ++i)
	{
		_vec.push_back(feb + std::to_string(i));
	}
	
}

void set_day_vec(std::vector<std::string>& _vec)
{
	std::string feb = "02";
	for (int i = 22; i <= 29; ++i)
	{
		_vec.push_back(feb + std::to_string(i));
	}
	std::string mar = "03";
	for (int i = 1; i <= 19; ++i)
	{
		if (i < 10)
			_vec.push_back(mar + "0" + std::to_string(i));
		else
			_vec.push_back(mar + std::to_string(i));
	}
}

void set_time_vec(std::vector<std::string> & _vec)
{
	for (int i = 7; i <= 20; ++i)
	{
		if (i < 10)
			_vec.push_back("0" + std::to_string(i));
		else
			_vec.push_back(std::to_string(i));
	}
}


bool isFormatTrue(std::string str)
{
	bool flag;

	for (int i = 0; i < 5; i++)
	{
		if (str.find_first_of(",") != std::string::npos)
		{
			int idx = str.find_first_of(",");
			str = str.substr(str.find_first_of(",") + 1, str.size() - idx);
		}
		else
			return false;
	}
	if (str.find_first_of(",") != std::string::npos)
		return false;
	else
		return true;
}

void file_name_vec(std::vector<std::string>& _vec, int _size)
{
	for(int idx=0; idx<_size;++idx)
		_vec.push_back(std::to_string(idx + 1)+".csv");
		
}

void doSystorParsing(std::string _file_name)
{
	std::string file_name = _file_name;

	// additional 20160216 - 20160221
	// day : 20160222 - 20160319
	// time : 07 - 20 
	std::vector<std::string> day_vec;
	std::vector<std::string> time_vec;
	set_day_vec(day_vec);

	set_time_vec(time_vec);


	//append all RUN I/O log in each time and sort it each. 
	for (int i = 0; i < day_vec.size(); ++i)
	{
		for (int j = 0; j < time_vec.size(); ++j)
		{


			std::string base_str = file_name;
			base_str += day_vec[i] + time_vec[j];
			std::ofstream intermediate_file(base_str + "-Total.csv", std::ios::app);

			append_each_storage(base_str, intermediate_file);
			intermediate_file.close();
			sort_total(base_str);

			std::cout << base_str << " finished disk aggregation\n";

		}
	}

	std::ofstream all_append_file("Systor17/systor17.csv", std::ios::app);
	for (int i = 0; i < day_vec.size(); ++i)
	{
		std::ofstream append_file_per_day(file_name + day_vec[i] + ".csv", std::ios::app);
		for (int j = 0; j < time_vec.size(); ++j)
		{
			std::string base_str = file_name + day_vec[i] + time_vec[j] + "-Total_sort.csv";



			append_each_sorted_file(base_str, all_append_file);
			append_each_sorted_file(base_str, append_file_per_day);
			std::cout << base_str + time_vec[j] << " finished appending\n";
		}
		parse_disk_page_IO(file_name + day_vec[i] + ".csv");
		std::cout << "finished " + file_name + day_vec[i] << "\n";
	}
	std::cout << "finished all appending data\n";


	//this is for final  
	//parse disk I/O access pattern to page cache access sequence 

	std::cout << "start aggregation for all dataset\n";
	parse_disk_page_IO("Systor17/systor17.csv");
	std::cout << "all_day appended parsing finished\n";



}


void append_only_diskio(
	std::string _file_name,
	std::vector<std::string>& _vec,
	std::ofstream& _out)
{
	//for (int i = 0; i < 1; ++i)
	for (int i = 0; i < _vec.size(); ++i)
	{
		std::string file_on_time = _file_name + _vec[i];

		std::cout << file_on_time << std::endl;

		std::ifstream each_file(file_on_time);

		if (!each_file.is_open())
			break;

		bool header_end_flag = false;
		while (!each_file.eof())
		{
			std::string one_line;
			getline(each_file, one_line);
			if (one_line == "")
				continue;
			if (one_line == "EndHeader")
				header_end_flag = true;

			if (header_end_flag == true)
			{
				if (one_line.find("DiskRead") != std::string::npos)
					_out << one_line << '\n';
				else if (one_line.find("DiskWrite") != std::string::npos)
					_out << one_line << '\n';
			}
			else
				continue;
		}
	}
}

std::string skip_one_comma(std::string& _one_line)
{
	u64_t idx = _one_line.find(",");
	if ( idx == std::string::npos)
		return "";
	else
	{
		std::string data = _one_line.substr(0, idx);
		_one_line = _one_line.substr(idx + 2, _one_line.size() - idx);
		return data;
	}
	
}
/**
  * @brief : parse timestamp, byteoffset, number of pages
  *
  */
void parse_byte_offset_page_num(std::string& _one_line, std::ofstream& out)
{
	
	skip_one_comma(_one_line);

	std::string timestamp_string = skip_one_comma(_one_line);
	if (timestamp_string == "")
		return;
	u64_t timestamp = stoull(timestamp_string, nullptr, 0);

	for (int idx = 0; idx < 3; ++idx)
		skip_one_comma(_one_line);

	std::string offset_string = skip_one_comma(_one_line);
	if (offset_string == "")
		return;
	u64_t byte_offset = strtoull(offset_string.c_str(), 0, 16);

	std::string num_page_string = skip_one_comma(_one_line);
	if (num_page_string == "")
		return;
	u64_t num_of_pages = strtoull(num_page_string.c_str(), 0, 16);

	num_of_pages = ( (num_of_pages % 4096 == 0) ? (num_of_pages / 4096) : ((num_of_pages / 4096) + 1) );

	skip_one_comma(_one_line);
	
	std::string disk_num_string = skip_one_comma(_one_line);
	if (disk_num_string == "")
		return;
	u64_t disk_num = stoull(disk_num_string, nullptr, 0);
	
	byte_offset = append_address((int)disk_num, byte_offset);


	out << timestamp << " " << byte_offset << " " << num_of_pages << std::endl;
	
}

void doParseforOffsetPageNum(std::ifstream& in, std::ofstream& out)
{
	out << "TimeStamp, ByteOffset, NumOfPages\n";

	if (in.is_open())
	{
		while (!in.eof())
		{
			std::string one_line;
			getline(in, one_line);
			parse_byte_offset_page_num(one_line, out);
		}
	}
}

void doProductionParsing(std::string& _file_path)
{
	std::string file_path = _file_path;
	std::string file_path_bs = file_path + "BuildServer/";
	std::string file_path_dtr = file_path + "DevelopmentToolsRelease/";
	std::string file_path_dads = file_path + "DisplayAdsDataServer/";
	std::string file_path_dap = file_path + "DisplayAdsPayload/";
	std::string file_path_lmbe = file_path + "LiveMapsBackEnd/";
	std::string file_path_msnscfs = file_path + "MSNStorageCFS/";
	std::string file_path_msnsfs = file_path + "MSNStorageFileServer/";
	std::string file_path_ra = file_path + "RadiusAuthentication/";
	std::string file_path_rbes = file_path + "RadiusBackEndSQLServer/";

	std::vector<std::string> bs_vec;
	std::vector<std::string> dtr_vec;
	std::vector<std::string> dads_vec;
	std::vector<std::string> dap_vec;
	std::vector<std::string> lmbe_vec;
	std::vector<std::string> msnscfs_vec;
	std::vector<std::string> msnsfs_vec;
	std::vector<std::string> ra_vec;
	std::vector<std::string> rbes_vec;
	
	//_size == # of log files
	file_name_vec(bs_vec, 96);
	file_name_vec(dtr_vec, 24);
	file_name_vec(dads_vec, 48);
	file_name_vec(dap_vec, 48);
	file_name_vec(lmbe_vec, 24);
	file_name_vec(msnscfs_vec, 36);
	file_name_vec(msnsfs_vec, 36);
	file_name_vec(ra_vec, 20);
	file_name_vec(rbes_vec, 17);

	//concatenating file name for each application type 
	std::string bs_append_file = file_path_bs + "diskio_output.csv";
	std::string dtr_append_file = file_path_dtr + "diskio_output.csv";
	std::string dads_append_file = file_path_dads + "diskio_output.csv";
	std::string dap_append_file = file_path_dap + "diskio_output.csv";
	std::string lmbe_append_file = file_path_lmbe + "diskio_output.csv";
	std::string msnscfs_append_file = file_path_msnscfs + "diskio_output.csv";
	std::string msnsfs_append_file = file_path_msnsfs + "diskio_output.csv";
	std::string ra_append_file = file_path_ra + "diskio_output.csv";
	std::string rbes_append_file = file_path_rbes + "diskio_output.csv";


	//constructing FILE output stream for each application type 
	std::ofstream bs_out(bs_append_file, std::ios::app);
	std::ofstream dtr_out(dtr_append_file, std::ios::app);
	std::ofstream dads_out(dads_append_file, std::ios::app);
	std::ofstream dap_out(dap_append_file, std::ios::app);
	std::ofstream lmbe_out(lmbe_append_file, std::ios::app);
	std::ofstream msnscfs_out(msnscfs_append_file, std::ios::app);
	std::ofstream msnsfs_out(msnsfs_append_file, std::ios::app);
	std::ofstream ra_out(ra_append_file, std::ios::app);
	std::ofstream rbes_out(rbes_append_file, std::ios::app);

	std::cout << "append bs server's log for only disk I/ O\n";
	append_only_diskio(file_path_bs, bs_vec, bs_out);
	std::cout << "append bs server's log for only disk I/O finished\n";
	bs_out.close();

	std::cout << "append dtr server's log for only disk I/ O\n";
	append_only_diskio(file_path_dtr, dtr_vec, dtr_out);
	std::cout << "append bs server's log for only disk I/O finished\n";
	dtr_out.close();

	std::cout << "append dads server's log for only disk I/ O\n";
	append_only_diskio(file_path_dads, dads_vec, dads_out);
	std::cout << "append dads server's log for only disk I/O finished\n";
	dads_out.close();

	std::cout << "append dap server's log for only disk I/ O\n";
	append_only_diskio(file_path_dap, dap_vec, dap_out);
	std::cout << "append bs server's log for only disk I/O finished\n";
	dap_out.close();

	std::cout << "append lmbe server's log for only disk I/ O\n";
	append_only_diskio(file_path_lmbe, lmbe_vec, lmbe_out);
	std::cout << "append lmbe server's log for only disk I/O finished\n";
	lmbe_out.close();

	std::cout << "append msnscfs server's log for only disk I/ O\n";
	append_only_diskio(file_path_msnscfs, msnscfs_vec, msnscfs_out);
	std::cout << "append bs server's log for only disk I/O finished\n";
	msnscfs_out.close();

	std::cout << "append msnsfs server's log for only disk I/ O\n";
	append_only_diskio(file_path_msnsfs, msnsfs_vec, msnsfs_out);
	std::cout << "append bs server's log for only disk I/O finished\n";
	msnsfs_out.close();

	std::cout << "append ra server's log for only disk I/ O\n";
	append_only_diskio(file_path_ra, ra_vec, ra_out);
	std::cout << "append ra server's log for only disk I/O finished\n";
	ra_out.close();

	std::cout << "append rbes server's log for only disk I/ O\n";
	append_only_diskio(file_path_rbes, rbes_vec, rbes_out);
	std::cout << "append bs server's log for only disk I/O finished\n";
	rbes_out.close();

	std::ofstream final_bs(file_path_bs + "final_bs.txt");
	std::ofstream final_dtr(file_path_dtr + "final_dtr.txt");
	std::ofstream final_dads(file_path_dads + "final_dads.txt");
	std::ofstream final_dap(file_path_dap + "final_dap.txt");
	std::ofstream final_lmbe(file_path_lmbe + "final_lmbe.txt");
	std::ofstream final_msnscfs(file_path_msnscfs + "final_msnscfs.txt");
	std::ofstream final_msnsfs(file_path_msnsfs + "final_msnsfs.txt");
	std::ofstream final_ra(file_path_ra + "final_ra.txt");
	std::ofstream final_rbes(file_path_rbes + "final_rbes.txt");

	std::ifstream bs_diskio(bs_append_file);
	std::ifstream dtr_diskio(dtr_append_file);
	std::ifstream dads_diskio(dads_append_file);
	std::ifstream dap_diskio(dap_append_file);
	std::ifstream lmbe_diskio(lmbe_append_file);
	std::ifstream msnscfs_diskio(msnscfs_append_file);
	std::ifstream msnsfs_diskio(msnsfs_append_file);
	std::ifstream ra_diskio(ra_append_file);
	std::ifstream rbes_diskio(rbes_append_file);


	std::cout << "parse -ongoing\n";
	doParseforOffsetPageNum(bs_diskio, final_bs);
	bs_diskio.close();
	final_bs.close();
	
	doParseforOffsetPageNum(dtr_diskio, final_dtr);
	dtr_diskio.close();
	final_dtr.close();

	doParseforOffsetPageNum(dads_diskio, final_dads);
	dads_diskio.close();
	final_dads.close();

	doParseforOffsetPageNum(dap_diskio, final_dap);
	dap_diskio.close();
	final_dap.close();

	doParseforOffsetPageNum(lmbe_diskio, final_lmbe);
	lmbe_diskio.close();
	final_lmbe.close();
	
	doParseforOffsetPageNum(msnscfs_diskio, final_msnscfs);
	msnscfs_diskio.close();
	final_msnscfs.close();
	
	doParseforOffsetPageNum(msnsfs_diskio, final_msnsfs);
	msnsfs_diskio.close();
	final_msnsfs.close();

	doParseforOffsetPageNum(ra_diskio, final_ra);
	ra_diskio.close();
	final_ra.close();

	doParseforOffsetPageNum(rbes_diskio, final_rbes);
	rbes_diskio.close();
	final_rbes.close();

	std::cout << "parse -finished\n";
}



void doEnterPriseParsing(std::string& _file_path)
{
	std::string file_path = _file_path;
	std::string file_path_exchange = file_path + "Exchange/";
	std::string file_path_TPCC1 = file_path + "TPCC1/";
	std::string file_path_TPCC2 = file_path + "TPCC2/";
	std::string file_path_TPCE = file_path + "TPCE/";

	std::vector<std::string> exchange_vec;
	std::vector<std::string> tpcc_vec;
	std::vector<std::string> tpcc_vec2;
	std::vector<std::string> tpce_vec;

	//_size == # of log files
	file_name_vec(exchange_vec,96);
	file_name_vec(tpcc_vec, 8);
	file_name_vec(tpcc_vec2, 6);
	file_name_vec(tpce_vec, 6);

	//concatenating file name for each application type 
	std::string exchange_append_file = file_path_exchange + "diskio_output.csv";
	std::string tpcc1_append_file = file_path_TPCC1 + "diskio_output.csv";
	std::string tpcc2_append_file = file_path_TPCC2 + "diskio_output.csv";
	std::string tpce_append_file = file_path_TPCE + "diskio_output.csv";

	//constructing FILE output stream for each application type 
	std::ofstream exchange_out(exchange_append_file, std::ios::app);
	std::ofstream tpcc1_out(tpcc1_append_file, std::ios::app);
	std::ofstream tpcc2_out(tpcc2_append_file, std::ios::app);
	std::ofstream tpce_out(tpce_append_file, std::ios::app);

	/*
	std::cout << "append exchange server's log for only disk I/ O\n";
	append_only_diskio(file_path_exchange, exchange_vec, exchange_out);
	std::cout << "append exchange server's log for only disk I/O finished\n";
	exchange_out.close();
	*/
	
	/*
	std::cout << "append tpcc1 server's log for only disk I/O\n";
	append_only_diskio(file_path_TPCC1, tpcc_vec, tpcc1_out);
	std::cout << "append tpcc1 server's log for only disk I/O finished\n";
	tpcc1_out.close();
	*/

	std::cout << "append tpcc2 server's log for only disk I/O\n";
	append_only_diskio(file_path_TPCC2, tpcc_vec2, tpcc2_out);
	std::cout << "append tpcc2 server's log for only disk I/O finished\n";
	tpcc2_out.close();


	std::cout << "append tpce server's log for only disk I/O\n";
	append_only_diskio(file_path_TPCE, tpce_vec, tpce_out);
	std::cout << "append tpce server's log for only disk I/O finished\n";
	tpce_out.close();
	
	
	std::ofstream final_exchange(file_path_exchange + "final_exchange.txt");
	std::ofstream final_tpcc1(file_path_TPCC1 + "final_tpcc1.txt");
	std::ofstream final_tpcc2(file_path_TPCC2 + "final_tpcc2.txt");
	std::ofstream final_tpce(file_path_TPCE + "final_tpce.txt");

	std::ifstream exchange_diskio(exchange_append_file);
	std::ifstream tpcc1_diskio(tpcc1_append_file);
	std::ifstream tpcc2_diskio(tpcc2_append_file);
	std::ifstream tpce_diskio(tpce_append_file);
	
	/*
	std::cout << "parse exchange -ongoing\n";
	doParseforOffsetPageNum(exchange_diskio, final_exchange);
	exchange_diskio.close();
	final_exchange.close();
	std::cout << "parse exchange -finished\n";
	
	std::cout << "parse tpcc1 -ongoing\n";
	doParseforOffsetPageNum(tpcc1_diskio, final_tpcc1);
	tpcc1_diskio.close();
	final_tpcc1.close();
	std::cout << "parse tpcc1 -finished\n";
	*/

	std::cout << "parse tpcc2 -ongoing\n";
	doParseforOffsetPageNum(tpcc2_diskio, final_tpcc2);
	tpcc2_diskio.close();
	final_tpcc2.close();
	std::cout << "parse tpcc2 -finished\n";

	std::cout << "parse tpce -ongoing\n";
	doParseforOffsetPageNum(tpce_diskio, final_tpce);
	tpce_diskio.close();
	final_tpce.close();
	std::cout << "parse tpce -finished\n";


	
}

void doFIUtraceParsing(std::string _file_path)
{
	std::string path_fiu = _file_path;
	std::vector<std::string> name_vec;

	//name_vec.push_back("homes/");
	//name_vec.push_back("mail/");
	name_vec.push_back("web-vm/");

	for (int idx = 0; idx < name_vec.size(); ++idx)
	{
		std::string each_trace;
		each_trace = path_fiu + name_vec[idx];

		std::ofstream output_file(each_trace + "output.txt", std::ios::app);
		output_file << "Timestamp, BlockOffset, NumOfPages\n";
		for (int j = 1; j <= 21; ++j)
		{
			std::string each_trace_per_day = each_trace + std::to_string(j) + ".blkparse";
			std::ifstream in(each_trace_per_day);
			
			if(in.is_open())
			{
				std::string tmp;
				while (!in.eof())
				{
					getline(in, tmp);
					u64_t timestamp; 
					u64_t block_offset;
					u64_t num_of_pages;

					uint32_t idx=0;

					if (tmp.find(" ") != std::string::npos)
					{
						idx = tmp.find(" ");
						std::string string_tsp = tmp.substr(0, idx);
						tmp = tmp.substr(idx + 1, tmp.size() - idx);
						timestamp = std::stoull(string_tsp, nullptr, 0);
					}
					else
						continue;
					if (tmp.find(" ") != std::string::npos)
					{
						idx = tmp.find(" ");
					}
					else
						continue;
					tmp = tmp.substr(idx + 1, tmp.size() - idx);

					if (tmp.find(" ") != std::string::npos)
					{
						idx = tmp.find(" ");
					}
					else
						continue;
					tmp = tmp.substr(idx + 1, tmp.size() - idx);
					
					if (tmp.find(" ") != std::string::npos)
					{
						idx = tmp.find(" ");
						std::string string_offset = tmp.substr(0, idx);
						tmp = tmp.substr(idx + 1, tmp.size() - idx);
						block_offset = std::stoull(string_offset, nullptr, 0);
					}
					else
						continue;
					if (tmp.find(" ") != std::string::npos)
					{
						idx = tmp.find(" ");
						std::string string_page_num = tmp.substr(0, idx);
						tmp = tmp.substr(idx + 1, tmp.size() - idx);
						u64_t page_per_512 = std::stoull(string_page_num, nullptr, 0);
						page_per_512 *= 512;
						num_of_pages = (page_per_512 % 4096 == 0) ? ((page_per_512) / 4096) : (page_per_512 / 4096) + 1;
						//if (num_of_pages != 1)
							//std::cout << "found not 1 page\n" << std::endl;
					}
					else
						continue;
					output_file << timestamp << " " << block_offset << " " << num_of_pages << '\n';
				}
			}
		}
		std::cout << "finished\n";
	}

}

int main(int argc, char* argv[])
{

	std::string file_path_systor = "Systor17/2016";
	
	std::string file_path_ms_enterprise = "Microsoft Enterprise/";

	std::string file_path_ms_production = "Microsoft Production Server/";
	
	std::string file_path_fiu_trace = "FIU trace/";

	//doSystorParsing(file_name_systor);

	//doProductionParsing(file_path_ms_production);
	
	//doEnterPriseParsing(file_path_ms_enterprise);

	//doFIUtraceParsing(file_path_fiu_trace);

	
	

	

	return 0;
}


