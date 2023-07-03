#pragma once

#include <functional>
#include <memory>
#include <list>
#include <mutex>

class SubscriptionToken {
public:
    SubscriptionToken(std::function<void()> cleanupCallback)
        : cleanupCallback_(std::move(cleanupCallback)) {}

    SubscriptionToken(const SubscriptionToken&) = delete;

    SubscriptionToken(SubscriptionToken&&) = default;

    virtual ~SubscriptionToken() {
        if (cleanupCallback_) cleanupCallback_(); // cleanupCallback_ can be empty after std::move of SubscriptionToken
    }
private:
    std::function<void()> cleanupCallback_;
};

template<typename T>
class Publisher : public std::enable_shared_from_this<Publisher<T>> {
public:
    using SubscriberCallback = std::function<bool(T)>;

    void publish(T content){
        std::lock_guard<std::mutex> lock(subscribersCallbacksMutex_);
        for (auto& callback : subscribersCallbacks_){
            callback(content);
        }
    }

    SubscriptionToken addSubscriber(SubscriberCallback callback) {
        std::lock_guard<std::mutex> lock(subscribersCallbacksMutex_);
        auto itr = subscribersCallbacks_.insert(subscribersCallbacks_.end(), callback);
        auto weak_this = this->weak_from_this();

        std::function<void()> cleanupCallback = [weak_this, itr]{
            if (auto strong_this = weak_this.lock()){
                std::lock_guard<std::mutex> lock(strong_this->subscribersCallbacksMutex_);
                strong_this->subscribersCallbacks_.erase(itr);
                std::cout << "Unsubscribed \n";
            }
            else{
                std::cout << "Try unsubscribe but publisher already gone. No operation needed\n";
            }
        };

        return SubscriptionToken(cleanupCallback);
    }

private:
    std::mutex subscribersCallbacksMutex_;
    std::list<SubscriberCallback> subscribersCallbacks_;
};