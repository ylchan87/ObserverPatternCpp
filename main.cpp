#include <string>
#include <memory>
#include <iostream>
#include <vector>

#include "publisher.h"

using namespace std::placeholders; //defines _1 _2 _3 etc for use in std::bind

class Press : public Publisher<std::string>{
public:
    void publishNews(std::string content) {
        this->publish(content);
    }
};

class Company : public Publisher<double>{
public:
    void paySalary(double amount) {
        this->publish(amount);
    }
};

class Person{
public:
    Person(std::string name): name_(name){};

    bool onNews(std::string content){
        std::cout << name_ << " received news " << content << std::endl;
        return true;
    }

    bool onGetPaid(double amount){
        std::cout << name_ << " received salary " << amount << std::endl;
        return true;
    }

    std::vector<SubscriptionToken> subscriptionTokens;

private:
    std::string name_;
};

int main(){

    // When creating subscriptionToken, we would make it hold a weak_ptr to reference the publisher 
    // so the publishers must be managed by shared_ptr, 
    auto bbcnews = std::make_shared<Press>();
    auto walmart = std::make_shared<Company>();

    auto personA = std::make_shared<Person>("personA");
    auto personB = std::make_shared<Person>("personB");
    
    // use personA.get() to get the raw pointer, 
    // else the subscriptionCallback will hold 1 ref_count to the shared_ptr personA
    // and prevent personA from destruction even whe we run "personA = nullptr;"
    {
        auto token1 = bbcnews->addSubscriber( std::bind(&Person::onNews   , personA.get() , _1 ) );
        auto token2 = walmart->addSubscriber( std::bind(&Person::onGetPaid, personA.get() , _1 ) );
        personA->subscriptionTokens.push_back(std::move(token1));
        personA->subscriptionTokens.push_back(std::move(token2));
    }

    
    personB->subscriptionTokens.push_back(
        bbcnews->addSubscriber( std::bind(&Person::onNews   , personB.get() , _1 ) )
    );
    personB->subscriptionTokens.push_back(
        walmart->addSubscriber( std::bind(&Person::onGetPaid, personB.get() , _1 ) )
    );
    
    
    std::cout << std::endl << "==== Case0: Normal pub sub" << std::endl;
    bbcnews->publishNews("News1");
    walmart->paySalary(10.0);
    
    std::cout << std::endl << "==== Case1: Subscriber dead before Publisher, subscription are canceled automatically" << std::endl;
    personA = nullptr;
    bbcnews->publishNews("News2");

    std::cout << std::endl << "==== Case2: Publisher closed before Subscriber, no segfault" << std::endl;
    bbcnews = nullptr;
    walmart->paySalary(9.0);

    return 0;
}