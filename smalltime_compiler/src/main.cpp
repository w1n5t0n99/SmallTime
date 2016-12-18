#include <iostream>
#include <fstream>
#include <Windows.h>
#include <vector>
#include <memory>

#include "..\include\comp_decls.h"
#include "..\include\Parser.h"
#include "..\include\generator.h"
#include "..\include\src_builder.h"
#include "..\include\file_builder.h"
#include "..\include\zone_post_generator.h"
#include "..\include\comp_logger.h"

#include <basic_datetime.h>
#include <tzdb_connector_interface.h>
#include "..\include\tzdb_raw_connector.h"

using namespace smalltime;

static bool log_transition_data = false;

int main()
{
	
	std::vector<std::string> vSrc = { "iana\\northamerica", "iana\\southamerica", "iana\\asia", "iana\\africa", "iana\\australasia", "iana\\antarctica", "iana\\europe" };
	std::vector<comp::ZoneData> vec_zonedata = {};
	std::vector<comp::RuleData> vec_ruledata = {};
	std::vector<comp::LinkData> vec_linkdata = {};

	std::vector<tz::Zone> vec_zone = {};
	std::vector<tz::Rule> vec_rule = {};
	std::vector<tz::Link> vec_link = {};

	std::vector<tz::Zones> vec_zone_lookup;
	std::vector<tz::Rules> vec_rule_lookup;
	comp::MetaData tzdb_meta;

	comp::Parser parser;
	comp::Generator generator;
	comp::SrcBuilder src_builder;
	comp::FileBuilder file_builder;

	std::ifstream inf;
	for (const auto& src : vSrc)
	{
		inf.open(src, std::ios::in);
		if (inf)
		{
			parser.ParseZones(vec_zonedata, inf);
			parser.ParseRules(vec_ruledata, inf);
			parser.ParseLinks(vec_linkdata, inf);
			std::cout << src << " parsed ..." << std::endl;
		}
		else
		{
			std::cout << "ERROR: " << src << " not parsed ..." << std::endl;

		}
		inf.close();
	}
	

	generator.ProcessZones(vec_zone, vec_zonedata);
	std::cout << "Zones processed ..." << std::endl;

	generator.ProcessRules(vec_rule, vec_ruledata);
	std::cout << "Rules processed ..." << std::endl;

	generator.ProcessLinks(vec_link, vec_linkdata);
	std::cout << "Links processed ..." << std::endl;

	generator.ProccessUtc(vec_zone);
	std::cout << "Utc processed ..." << std::endl;

	generator.ProcessZoneLookup(vec_zone_lookup, vec_zone, vec_link);
	std::cout << "Zone lookup processed ..." << std::endl;

	generator.ProcessRuleLookup(vec_rule_lookup, vec_rule);
	std::cout << "Rule lookup processed ..." << std::endl;

	generator.ProcessMeta(tzdb_meta, vec_zone, vec_rule, vec_zone_lookup, vec_rule_lookup);
	std::cout << "Metadata processed ..." << std::endl;

	std::shared_ptr<comp::TzdbRawConnector> tzdb_connector = std::make_shared<comp::TzdbRawConnector>(tzdb_meta, vec_zone, vec_rule, vec_zone_lookup, vec_rule_lookup);
	comp::ZonePostGenerator zone_post_generator(tzdb_connector);


	zone_post_generator.ProcessZones(vec_zone);
	std::cout << "Zone post-processed ..." << std::endl;

	std::ofstream outf("Tzdb.h", std::ofstream::trunc);
	src_builder.BuildHead(outf);
	src_builder.BuildBody(vec_rule, vec_zone, vec_zone_lookup, vec_rule_lookup, tzdb_meta, outf);
	src_builder.BuildTail(outf);

	//std::ofstream outf("tzdb.bin", std::ios::out | std::ios::binary);
	//file_builder.Build(vec_rule, vec_zone, vec_zone_lookup, vec_rule_lookup, outf);

	outf.close();
	std::cout << "Source compiled ..." << std::endl;

	comp::CompLogger comp_logger;
	comp_logger.LogAllZones(std::cout, vec_zone, vec_zonedata);
	//comp_logger.LogZoneData(std::cout, vec_zone, vec_zonedata, "Australia/Perth");

	

	std::cin.get();
	return 0;
}