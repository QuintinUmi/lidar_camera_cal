#include <ros/ros.h>
#include <fstream>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Point32.h>
#include <pcl/point_cloud.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_types.h>
#include <pcl/PCLPointCloud2.h>

#include <Eigen/Dense>

#include <yaml-cpp/yaml.h>

#include "tools/file_operator.h"

using namespace lcr_cal;


PointSetCsvOperator::PointSetCsvOperator(std::string file_path)
{   
    this->setPath(file_path);
}

PointSetCsvOperator::~PointSetCsvOperator(){}


void PointSetCsvOperator::setPath(std::string file_path)
{
    this->file_path_ = file_path;
}

void PointSetCsvOperator::writePointsToCSVOverwrite(const std::vector<geometry_msgs::Point32>& group1, const std::vector<geometry_msgs::Point32>& group2) 
{
    std::ofstream outFile(this->file_path_, std::ofstream::trunc);  
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open file for writing.\n";
        return;
    }
    
    outFile << "x1,y1,z1,x2,y2,z2\n";
    for (size_t i = 0; i < group1.size() && i < group2.size(); ++i) {
        outFile << group1[i].x << "," << group1[i].y << "," << group1[i].z << ","
                << group2[i].x << "," << group2[i].y << "," << group2[i].z << "\n";
    }
    
    outFile.close();
    std::cout << "Data successfully written to " << this->file_path_ << " (overwritten)\n";
}



void PointSetCsvOperator::writePointsToCSVAppend(const std::vector<geometry_msgs::Point32>& group1, const std::vector<geometry_msgs::Point32>& group2) 
{
    std::ofstream outFile(this->file_path_, std::ofstream::app);  
    if(!outFile.is_open()) 
    {
        std::cerr << "Error: Unable to open file for writing.\n";
        return;
    }
    
    for(size_t i = 0; i < group1.size() && i < group2.size(); ++i) 
    {
        outFile << group1[i].x << "," << group1[i].y << "," << group1[i].z << ","
                << group2[i].x << "," << group2[i].y << "," << group2[i].z << "\n";
    }
    
    outFile.close();
    std::cout << "Data successfully appended to " << this->file_path_ << "\n";
}




void PointSetCsvOperator::deleteRowFromCSV(size_t rowIndex) 
{
    std::ifstream inFile(this->file_path_);
    std::vector<std::string> lines;
    std::string line;
    
    if(!inFile.is_open()) 
    {
        std::cerr << "Error: Unable to open file for reading.\n";
        return;
    }
    
    while(getline(inFile, line)) 
    {
        lines.push_back(line);
    }
    
    inFile.close();

    if(rowIndex < 1 || rowIndex >= lines.size()) 
    {
        std::cerr << "Error: Row index out of range.\n";
        return;
    }
    
    std::ofstream outFile(this->file_path_);
    if(!outFile.is_open()) 
    {
        std::cerr << "Error: Unable to open file for writing.\n";
        return;
    }
    
    for(size_t i = 0; i < lines.size(); ++i) 
    {
        if (i != rowIndex) {
            outFile << lines[i] << "\n";
        }
    }
    
    outFile.close();
    std::cout << "Row " << rowIndex << " deleted successfully from " << this->file_path_ << "\n";
}

void PointSetCsvOperator::readPointsFromCSV(std::vector<geometry_msgs::Point32>& group1, std::vector<geometry_msgs::Point32>& group2) 
{
    group1.clear();
    group2.clear();

    std::ifstream inFile(this->file_path_);
    if (!inFile.is_open()) 
    {
        std::cerr << "Error: Unable to open file for reading.\n";
        return;
    }
    
    // Skip the header
    std::string line;
    std::getline(inFile, line);
    
    // Read the data
    while (std::getline(inFile, line)) 
    {
        std::stringstream ss(line);
        geometry_msgs::Point32 point1, point2;
        char delim;
        
        ss >> point1.x >> delim >> point1.y >> delim >> point1.z >> delim
           >> point2.x >> delim >> point2.y >> delim >> point2.z;
        
        group1.push_back(point1);
        group2.push_back(point2);
    }
    
    inFile.close();
    std::cout << "Data successfully read from " << this->file_path_ << std::endl;
}














BorderSetCsvOperator::BorderSetCsvOperator(std::string file_path)
{   
    this->setPath(file_path);
}

BorderSetCsvOperator::~BorderSetCsvOperator(){}


void BorderSetCsvOperator::setPath(std::string file_path)
{
    this->file_path_ = file_path;
}


void BorderSetCsvOperator::writePointsToCSVOverwrite(const pcl::PointCloud<pcl::PointXYZI>::Ptr group1, const std::vector<geometry_msgs::Point32>& group2) 
{
    std::ofstream outFile(this->file_path_, std::ofstream::trunc);  
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open file for writing.\n";
        return;
    }
    
    outFile << "x,y,z,x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4\n";

    size_t size_gp1 = group1->points.size();
    for (size_t i = 0; i < size_gp1; ++i) {
        outFile << group1->points[i].x << "," << group1->points[i].y << "," << group1->points[i].z << ","
                << group2[0].x << "," << group2[0].y << "," << group2[0].z 
                << group2[1].x << "," << group2[1].y << "," << group2[1].z 
                << group2[2].x << "," << group2[2].y << "," << group2[2].z 
                << group2[3].x << "," << group2[3].y << "," << group2[3].z << "\n";
    }
    
    outFile.close();
    std::cout << "Data successfully written to " << this->file_path_ << " (overwritten)\n";
}
void BorderSetCsvOperator::writePointsToCSVOverwrite(const pcl::PointCloud<pcl::PointXYZI>::Ptr group1, const std::vector<std::vector<geometry_msgs::Point32>>& group2) 
{
    std::ofstream outFile(this->file_path_, std::ofstream::trunc);  
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open file for writing.\n";
        return;
    }
    
    outFile << "x,y,z,x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4\n";

    size_t minSize = std::min(group1->points.size(), group2.size());
    for (size_t i = 0; i < minSize; ++i) {
        outFile << group1->points[i].x << "," << group1->points[i].y << "," << group1->points[i].z << ","
                << group2[i][0].x << "," << group2[i][0].y << "," << group2[i][0].z 
                << group2[i][1].x << "," << group2[i][1].y << "," << group2[i][1].z 
                << group2[i][2].x << "," << group2[i][2].y << "," << group2[i][2].z 
                << group2[i][3].x << "," << group2[i][3].y << "," << group2[i][3].z << "\n";
    }
    
    outFile.close();
    std::cout << "Border Data successfully written to " << this->file_path_ << " (overwritten)\n";
}



void BorderSetCsvOperator::writePointsToCSVAppend(const pcl::PointCloud<pcl::PointXYZI>::Ptr group1, const std::vector<geometry_msgs::Point32>& group2) 
{
    std::ofstream outFile(this->file_path_, std::ofstream::app);  
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open file for writing.\n";
        return;
    }
    
    size_t size_gp1 = group1->points.size();
    for (size_t i = 0; i < size_gp1; ++i) {
        outFile << group1->points[i].x << "," << group1->points[i].y << "," << group1->points[i].z << ","
                << group2[0].x << "," << group2[0].y << "," << group2[0].z 
                << group2[1].x << "," << group2[1].y << "," << group2[1].z 
                << group2[2].x << "," << group2[2].y << "," << group2[2].z 
                << group2[3].x << "," << group2[3].y << "," << group2[3].z << "\n";
    }
    
    outFile.close();
    std::cout << "Border Data successfully appended to " << this->file_path_ << "\n";
}
void BorderSetCsvOperator::writePointsToCSVAppend(const pcl::PointCloud<pcl::PointXYZI>::Ptr group1, const std::vector<std::vector<geometry_msgs::Point32>>& group2) 
{
    std::ofstream outFile(this->file_path_, std::ofstream::app);  
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open file for writing.\n";
        return;
    }
    
    size_t minSize = std::min(group1->points.size(), group2.size());
    for (size_t i = 0; i < minSize; ++i) {
        outFile << group1->points[i].x << "," << group1->points[i].y << "," << group1->points[i].z << ","
                << group2[i][0].x << "," << group2[i][0].y << "," << group2[i][0].z 
                << group2[i][1].x << "," << group2[i][1].y << "," << group2[i][1].z 
                << group2[i][2].x << "," << group2[i][2].y << "," << group2[i][2].z 
                << group2[i][3].x << "," << group2[i][3].y << "," << group2[i][3].z << "\n";
    }
    
    outFile.close();
    std::cout << "Data successfully appended to " << this->file_path_ << "\n";
}



void BorderSetCsvOperator::deleteRowFromCSV(size_t rowIndex) 
{
    std::ifstream inFile(this->file_path_);
    std::vector<std::string> lines;
    std::string line;
    
    if(!inFile.is_open()) 
    {
        std::cerr << "Error: Unable to open file for reading.\n";
        return;
    }
    
    while(getline(inFile, line)) 
    {
        lines.push_back(line);
    }
    
    inFile.close();

    if(rowIndex < 1 || rowIndex >= lines.size()) 
    {
        std::cerr << "Error: Row index out of range.\n";
        return;
    }
    
    std::ofstream outFile(this->file_path_);
    if(!outFile.is_open()) 
    {
        std::cerr << "Error: Unable to open file for writing.\n";
        return;
    }
    
    for(size_t i = 0; i < lines.size(); ++i) 
    {
        if (i != rowIndex) {
            outFile << lines[i] << "\n";
        }
    }
    
    outFile.close();
    std::cout << "Row " << rowIndex << " deleted successfully from " << this->file_path_ << "\n";
}
void BorderSetCsvOperator::deleteLastSetFromCSV() 
{
    std::ifstream inFile(this->file_path_);
    std::vector<std::string> lines;
    std::string line;
    
    if(!inFile.is_open()) 
    {
        std::cerr << "Error: Unable to open file for reading.\n";
        return;
    }
    
    while(getline(inFile, line)) 
    {
        lines.push_back(line);
    }
    
    inFile.close();

    int borderset_size = getBordersetSize();
    
    std::ofstream outFile(this->file_path_);
    if(!outFile.is_open()) 
    {
        std::cerr << "Error: Unable to open file for writing.\n";
        return;
    }
    


    pcl::PointCloud<pcl::PointXYZI>::Ptr group1;
    std::vector<std::vector<geometry_msgs::Point32>> group2;

    readPointsFromCSV(group1, group2);

    if(group2.size() == 0)
    {
        return;
    }

    int size_set = 1;
    std::vector<geometry_msgs::Point32> last_gp2 = group2[0];
    for(size_t i = 0; i < lines.size(); ++i) 
    {
        if(last_gp2 != group2[i]) {
            size_set ++;
            last_gp2 = group2[i];
        }
        if(size_set >= borderset_size) {
            break;
        }    

        outFile << lines[i] << "\n";

    }
    
    outFile.close();
    std::cout << "Set " << borderset_size << " deleted successfully from " << this->file_path_ << "\n";
}


void BorderSetCsvOperator::readPointsFromCSV(pcl::PointCloud<pcl::PointXYZI>::Ptr group1, std::vector<std::vector<geometry_msgs::Point32>>& group2) 
{
    group1->clear();
    group2.clear();

    std::ifstream inFile(this->file_path_);
    if (!inFile.is_open()) {
        std::cerr << "Error: Unable to open file for reading.\n";
        return;
    }
    
    // Skip the header
    std::string line;
    std::getline(inFile, line);
    
    // Read the data
    while (std::getline(inFile, line)) {
        std::stringstream ss(line);
        pcl::PointXYZI point1;
        std::vector<geometry_msgs::Point32> point2;
        geometry_msgs::Point32 point2_tmp[4];
        char delim;

        ss >> point1.x >> delim >> point1.y >> delim >> point1.z >> delim
            >> point2_tmp[0].x >> delim >> point2_tmp[0].y >> delim >> point2_tmp[0].z
            >> point2_tmp[1].x >> delim >> point2_tmp[1].y >> delim >> point2_tmp[1].z
            >> point2_tmp[2].x >> delim >> point2_tmp[2].y >> delim >> point2_tmp[2].z
            >> point2_tmp[3].x >> delim >> point2_tmp[3].y >> delim >> point2_tmp[3].z;
        
        group1->points.push_back(point1);
        for(int i = 0; i < 4; i++) {
            point2.emplace_back(point2_tmp[i]);
        }
        group2.emplace_back(point2);
        
    }
    
    inFile.close();
    std::cout << "Data successfully read from " << this->file_path_ << std::endl;
}


int BorderSetCsvOperator::getBordersetSize()
{
    pcl::PointCloud<pcl::PointXYZI>::Ptr group1;
    std::vector<std::vector<geometry_msgs::Point32>> group2;

    readPointsFromCSV(group1, group2);

    if(group2.size() == 0)
    {
        return 0;
    }

    int size_set = 1;
    std::vector<geometry_msgs::Point32> last_gp2 = group2[0];
    for(const auto& row: group2)
    {
        if(last_gp2 != row) {
            size_set ++;
            last_gp2 = row;
        }

    }
    
    return size_set;
}











YamlOperator::YamlOperator(std::string file_path)
{
    this->setPath(file_path);
}
YamlOperator::~YamlOperator(){}

void YamlOperator::setPath(std::string file_path)
{
    this->file_path_ = file_path;
}

void YamlOperator::ensureFileExists(const std::string& filename) 
{
    std::ifstream f(filename.c_str());
    if (!f.good()) {
        std::ofstream nf(filename.c_str());
        if (nf.good()) {
            nf << "root: {}" << std::endl;
        }
        nf.close();
        std::cout << "File created: " << filename << std::endl;
    } else {
        std::cout << "File exists: " << filename << std::endl;
    }
    f.close();
}

void YamlOperator::writeExtrinsicsToYaml(const Eigen::Matrix3f& R, const Eigen::Vector3f& t)
{
    YamlOperator::ensureFileExists(this->file_path_);

    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Rotation" << YAML::Value << YAML::Flow << YAML::BeginSeq;
    for (int i = 0; i < R.rows(); ++i) {
        for (int j = 0; j < R.cols(); ++j) {
            out << R(i, j);
        }
    }
    out << YAML::EndSeq;

    out << YAML::Key << "Translation" << YAML::Value << YAML::Flow << YAML::BeginSeq;
    for (int i = 0; i < t.size(); ++i) {
        out << t(i);
    }
    out << YAML::EndSeq;
    out << YAML::EndMap;

    // Write to file
    std::ofstream fout(this->file_path_);
    fout << out.c_str();
    fout.close();
}
bool YamlOperator::readExtrinsicsFromYaml(Eigen::Matrix3f& R, Eigen::Vector3f& t)
{
    try {
        YAML::Node config = YAML::LoadFile(this->file_path_);
        if (!config["Rotation"] || !config["Translation"]) {
            std::cerr << "Missing 'Rotation' or 'Translation' in YAML file." << std::endl;
            return false;
        }

        // Load rotation
        int idx = 0;
        for (int i = 0; i < R.rows(); ++i) {
            for (int j = 0; j < R.cols(); ++j) {
                R(i, j) = config["Rotation"][idx++].as<float>();
            }
        }

        // Load translation
        for (int i = 0; i < t.size(); ++i) {
            t(i) = config["Translation"][i].as<float>();
        }
    } catch (const YAML::Exception& e) {
        std::cerr << "Error loading YAML file: " << e.what() << std::endl;
        return false;
    }
    return true;
}
