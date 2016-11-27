#include <iostream>
#include <fstream>
#include <Windows.h>
#include <vector>
#include <memory>

#include "CompDecls.h"
#include "Parser\Parser.h"
#include "Generator\Generator.h"
#include "Builder\SrcBuilder.h"
#include <BasicDateTime.h>
#include <itzdbconnector.h>
#include "tzdb_raw_connector.h"

using namespace smalltime;

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
	comp::SrcBuilder srcBuilder;

	std::ifstream inf;
	for (const auto& src : vSrc)
	{
		inf.open(src, std::ios::in);
		if (inf)
		{
			parser.parseZones(vec_zonedata, inf);
			parser.parseRules(vec_ruledata, inf);
			parser.parseLinks(vec_linkdata, inf);
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

	generator.PostProcessZones(vec_zone, tzdb_connector);
	std::cout << "Zone post-processed ..." << std::endl;


	std::ofstream outf("Tzdb.h", std::ofstream::trunc);
	srcBuilder.buildHead(outf);
	srcBuilder.buildBody(vec_rule, vec_zone, vec_zone_lookup, vec_rule_lookup, tzdb_meta, outf);
	srcBuilder.buildTail(outf);

	outf.close();
	std::cout << "Source compiled ..." << std::endl;


	std::cin.get();
	return 0;
}