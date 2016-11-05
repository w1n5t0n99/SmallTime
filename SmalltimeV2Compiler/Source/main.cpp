#include <iostream>
#include <fstream>
#include <Windows.h>
#include <vector>

#include "CompDecls.h"
#include "Parser\Parser.h"
#include "Generator\Generator.h"
#include "Builder\SrcBuilder.h"
#include <BasicDateTime.h>

using namespace smalltime;

int main()
{
	std::vector<std::string> vSrc = { "iana\\northamerica", "iana\\southamerica", "iana\\asia", "iana\\africa", "iana\\australasia", "iana\\antarctica", "iana\\europe" };
	std::vector<comp::ZoneData> vZoneData = {};
	std::vector<comp::RuleData> vRuleData = {};
	std::vector<comp::LinkData> vLinkData = {};

	std::vector<tz::Zone> vZone = {};
	std::vector<tz::Rule> vRule = {};
	std::vector<tz::Link> vLink = {};

	comp::Parser parser;
	comp::Generator generator;
	comp::SrcBuilder srcBuilder;

	std::ifstream inf;
	for (const auto& src : vSrc)
	{
		inf.open(src, std::ios::in);
		if (inf)
		{
			parser.parseZones(vZoneData, inf);
			parser.parseRules(vRuleData, inf);
			parser.parseLinks(vLinkData, inf);
			std::cout << src << " parsed ..." << std::endl;
		}
		else
		{
			std::cout << "ERROR: " << src << " not parsed ..." << std::endl;

		}
		inf.close();
	}
	

	generator.processZones(vZone, vZoneData);
	std::cout << "Zones processed ..." << std::endl;

	generator.processRules(vRule, vRuleData);
	std::cout << "Rules processed ..." << std::endl;

	generator.processLinks(vLink, vLinkData);
	std::cout << "Links processed ..." << std::endl;

	generator.proccessUtc(vZone);
	std::cout << "Utc processed ..." << std::endl;



	std::ofstream outf("Tzdb.h", std::ofstream::trunc);
	srcBuilder.buildHead(outf);
	srcBuilder.buildBody(vRule, vZone, vLink, outf);
	srcBuilder.buildTail(outf);

	outf.close();
	std::cout << "Source compiled ..." << std::endl;


	std::cin.get();
	return 0;
}