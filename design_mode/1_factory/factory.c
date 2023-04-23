#include <stdio.h>

// 定义汽车类型
typedef enum {
    SEDAN,
    SPORTS,
    SUV
} CarType;

// 定义汽车结构体
typedef struct {
    CarType type;
    char* brand;
    char* model;
} Car;

// 定义工厂接口
typedef struct {
    Car* (*createCar)(CarType type);
} CarFactory;

// 定义具体工厂实现
typedef struct {
    CarFactory factory;
} CarFactoryImpl;

// 实现工厂接口方法
Car* createSedan() {
    Car* car = malloc(sizeof(Car));
    car->type = SEDAN;
    car->brand = "Toyota";
    car->model = "Camry";
    return car;
}

Car* createSportsCar() {
    Car* car = malloc(sizeof(Car));
    car->type = SPORTS;
    car->brand = "Porsche";
    car->model = "911";
    return car;
}

Car* createSUV() {
    Car* car = malloc(sizeof(Car));
    car->type = SUV;
    car->brand = "Jeep";
    car->model = "Wrangler";
    return car;
}

// 实现工厂接口方法
Car* createCar(CarFactory* factory, CarType type) {
    CarFactoryImpl* impl = (CarFactoryImpl*) factory;
    switch (type) {
        case SEDAN:
            return impl->factory.createCar = createSedan();
        case SPORTS:
            return impl->factory.createCar = createSportsCar();
        case SUV:
            return impl->factory.createCar = createSUV();
        default:
            return NULL;
    }
}

int main() {
    // 创建工厂对象
    CarFactoryImpl factory;
    factory.factory.createCar = createCar;

    // 创建不同类型的汽车
    Car* sedan = factory.factory.createCar(&factory.factory, SEDAN);
    Car* sportsCar = factory.factory.createCar(&factory.factory, SPORTS);
    Car* suv = factory.factory.createCar(&factory.factory, SUV);

    // 打印汽车信息
    printf("Sedan: %s %s\n", sedan->brand, sedan->model);
    printf("Sports car: %s %s\n", sportsCar->brand, sportsCar->model);
    printf("SUV: %s %s\n", suv->brand, suv->model);

    // 释放内存
    free(sedan);
    free(sportsCar);
    free(suv);

    return 0;
}

