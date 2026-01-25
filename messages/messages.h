#ifndef REQUESTS
#define REQUESTS


typedef enum RequestType {
    REQUEST_POWER,
    RETURN_POWER
}RequestType;

typedef struct Request{
    RequestType request_type;
    float power_amount;
}Request;



typedef enum ResponseType{
    APPROVED,
    REJECTED
}ResponseType;

typedef struct Response{
    ResponseType response_type;
    float power_amount;
}Response;

#endif