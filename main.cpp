#define NONIUS_RUNNER
#include <nonius/nonius_single.h++>
#include <string>
#include <fstream>

#include <yaml-cpp/yaml.h>

#include <yaml.h>
#include <stdio.h>
#include <libfyaml.h>
#include <string_view>

const char* YamlFileName = "File.yaml";


NONIUS_BENCHMARK("Load yaml-cpp", [](nonius::chronometer meter)
{
    std::ifstream FileStream(YamlFileName); 
    std::string FileText = {std::istreambuf_iterator<char>(FileStream),
          std::istreambuf_iterator<char>()}; 

    meter.measure([&FileText]() {
      YAML::Node YamlNode = YAML::Load(FileText); });
})

NONIUS_BENCHMARK("Save yaml-cpp",[](nonius::chronometer meter)
{
    YAML::Node YamlNode = YAML::LoadFile(YamlFileName);

    meter.measure([&YamlNode]()
    {
        std::ostringstream StringStream; 
        StringStream << YamlNode;
    });
})

NONIUS_BENCHMARK("Load libYaml", [](nonius::chronometer meter)
{
    std::ifstream FileStream(YamlFileName);
    std::string FileText = {std::istreambuf_iterator<char>(FileStream),
          std::istreambuf_iterator<char>()};

    meter.measure([&FileText]() {
        yaml_document_t document; 
        yaml_parser_t parser; 
        yaml_parser_initialize(&parser); 
        yaml_parser_set_input_string(&parser, (unsigned char*)FileText.c_str(), FileText.length());  
        yaml_parser_load(&parser, &document); 
        yaml_document_delete(&document); 
        yaml_parser_delete(&parser); 
    });
})

NONIUS_BENCHMARK("Save libYaml", [](nonius::chronometer meter)
{
    std::ifstream FileStream(YamlFileName);
    std::string FileText = {std::istreambuf_iterator<char>(FileStream),
          std::istreambuf_iterator<char>()};


    std::vector<yaml_document_t> Documents(meter.runs());
    for (yaml_document_t& Document : Documents)
    {
        yaml_parser_t parser;
        yaml_parser_initialize(&parser);
        yaml_parser_set_input_string(&parser, (unsigned char*)FileText.c_str(), FileText.length());
        yaml_parser_load(&parser, &Document);
        yaml_parser_delete(&parser);
    }

    auto writeHandler = [](void *data, unsigned char *buffer, size_t size)
    {
        try{
        std::ostringstream* Out = (std::ostringstream*) data; 
        *Out << std::string_view((char*)buffer, size);
        } catch (...) {return 0;}
        return 1;
    };

    int i = 0;
    meter.measure([&writeHandler, &Documents, &i]()
    {
        yaml_emitter_t emitter; 
        yaml_emitter_initialize(&emitter); 
        std::ostringstream Out;
        yaml_emitter_set_output(&emitter, writeHandler, &Out); 
        //yaml_emitter_set_output(&emitter, writeHandler, &Out);
        yaml_emitter_dump(&emitter, &Documents[i]); 
        yaml_emitter_delete(&emitter);
        ++i;
    });
})

NONIUS_BENCHMARK("Load libFYaml", [](nonius::chronometer meter)
{
    //fy_parse_cfg parserCfg;

    std::ifstream FileStream(YamlFileName);
    std::string FileText = {std::istreambuf_iterator<char>(FileStream),
          std::istreambuf_iterator<char>()};


    meter.measure([&FileText]() {
        fy_document* Document;
        Document = fy_document_build_from_string(NULL, FileText.c_str(), FileText.length()); 
        fy_document_destroy(Document);
    });

})

NONIUS_BENCHMARK("Save libFYaml", [](nonius::chronometer meter)
{
    //fy_parse_cfg parserCfg;

    std::ifstream FileStream(YamlFileName);
    std::string FileText = {std::istreambuf_iterator<char>(FileStream),
          std::istreambuf_iterator<char>()};
    std::vector<fy_document*> Documents(meter.runs());
    for (fy_document*& Document : Documents)
    {
        Document = fy_document_build_from_string(NULL, FileText.c_str(), FileText.length());
    }

    int i = 0;
    meter.measure([&Documents,&i]()
    {
        const char* str = fy_emit_document_to_string(Documents[i], fy_emitter_cfg_flags(0));
        //std::cout << str << std::endl;
        std::free((void*)str);
        ++i;
    });

    for (fy_document*& Document : Documents)
    {
        fy_document_destroy(Document);
    }


})
