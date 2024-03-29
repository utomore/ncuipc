#include "tako/keypoints.hpp"
namespace tako
{
    KeyPoints::KeyPoints(DBoW3::Vocabulary &vocab)
    {
        this->vocab_ = vocab;
    }

    KeyPoints::KeyPoints(DBoW3::Vocabulary &vocab, DBoW3::Database &db)
    {
        this->vocab_ = vocab;
    }

    void KeyPoints::setVocabulary(std::vector<tako::Node> &nodes)
    {
        std::cout<<"reading database" << std::endl;
        if(vocab_.empty())
        {
            std::cerr<<"Vocabulary does not exist . Building now" <<std::endl;
            this->create_database(nodes);
        }
        else
        {
            std::cout<<"Vocabulary load finish" <<std::endl;
        }
        
    }

    void KeyPoints::create_database(std::vector<tako::Node> &nodes)
    {
        for(Node& node:nodes)
        {
            std::vector<cv::KeyPoint> keypoints;
            cv::Mat descriptor;
            detector_->detectAndCompute(node.getImage(), cv::Mat(), keypoints, descriptor);
            node.descriptor_ = descriptor;
            descriptors_.push_back( descriptor );
        }
        std::cout<< "create Vocabulary..." <<std::endl;
        vocab_.create(descriptors_);
        std::cout<< "vocabulary info : " << vocab_ <<std::endl;

        vocab_.save("vocabulary.yml.gz");

        std::cout<< "save dictionary to file : vocabulary.yml.gz " <<std::endl;
    }

    double KeyPoints::compare_Image2Image(tako::Node node1, tako::Node node2)
    {
        //std::cout<<"compare image " <<node1.id_ <<" vs "<< node2.id_ <<std::endl;
        DBoW3::BowVector v1;
        vocab_.transform(node1.descriptor_, v1);
        DBoW3::BowVector v2;
        vocab_.transform(node2.descriptor_, v2);
        return vocab_.score(v1, v2);
    }
    
    // spatial compare
    double KeyPoints::compare_spatial2spatial(std::list<tako::Node> cluster_test, std::vector<tako::Node> cluster_dest)
    {
        double scoring = 0;
        tako::node node_1;
        tako::node node_2;
        for(std::list<tako::Node>::iterator iter = cluster_test.begin(); iter != cluster_test.end(); iter++)
        {
            node_1.descriptor_ = node_1.descriptor_ + iter.descriptor_;
        }
        for(tako::Node &node:cluster_dest)
        {
            node_2.descriptor_ = node_2.descriptor_ + iter.descriptor_;
                
        }
        scoring = this->compare_Image2Image(node_1, node_2);

        return scoring ;

    }
    //void KeyPoints::setDatabase()
    DBoW3::Database KeyPoints::setDatabase()
    {
        // database setting
        DBoW3::Database db_(vocab_, false, 0);
        for( int i = 0; i < this->descriptors_.size(); i++)
        {
            db_.add(this->descriptors_[i]);
        }
        std::cout << "database info : " << db_ << std::endl;
        return db_;
    }

    //void KeyPoints::compare_Image2Database(tako::Node &node, DBoW3::Database &db) 
    void KeyPoints::compare_Image2Database(tako::Node &node, DBoW3::Database &db, DBoW3::QueryResults &ret) 
    {
        //std::cout<<"compare image with database..." <<std::endl;
        // compare
        //std::cout<< " database info : "<< db_ << std::endl;
        db.query( node.descriptor_, ret, 20);
        //std::cout<< "searching image " << node.id_ << " return " <<ret<<std::endl;
            //if( std::abs(node.id_ - iter->Id) < 10)
            //{
             //   continue;
            //}
            //else
            //{
            //    break;
            //}
        
    }
}