#include <stdio.h>
#include <stdlib.h>

// 定义产品接口
typedef struct _Product Product;

struct _Product{
  void (*print)(Product *product);
};

// 实现产品接口的具体产品
typedef struct {
  Product product;
  int value;
} ConcreteProduct;

// 实现产品接口的具体产品
typedef struct {
  Product product;
  char *value;
} AnotherConcreteProduct;

// 实现工厂接口
typedef struct {
  Product *(*createProduct)(void);
} Factory;

// 实现工厂接口的具体工厂
typedef struct {
  Factory factory;
  int value;
} ConcreteFactory;

// 实现工厂接口的具体工厂
typedef struct {
  Factory factory;
  char *value;
} AnotherConcreteFactory;

// 实现具体产品的 print 方法
void ConcreteProduct_print(Product *product) {
  printf("Concrete product with value %d\n", ((ConcreteProduct *)product)->value);
}

// 实现具体产品的 print 方法
void AnotherConcreteProduct_print(Product *product) {
  printf("Another concrete product with value %s\n", ((AnotherConcreteProduct *)product)->value);
}

// 实现具体工厂的 createProduct 方法
Product *ConcreteFactory_createProduct(void) {
  ConcreteProduct *product = malloc(sizeof(ConcreteProduct));
  product->product.print = ConcreteProduct_print;
  product->value = 42;
  return (Product *)product;
}

// 实现具体工厂的 createProduct 方法
Product *AnotherConcreteFactory_createProduct(void) {
  AnotherConcreteProduct *product = malloc(sizeof(AnotherConcreteProduct));
  product->product.print = AnotherConcreteProduct_print;
  product->value = "Hello, world!";
  return (Product *)product;
}

int main() {
  // 创建具体工厂
  ConcreteFactory *factory1 = malloc(sizeof(ConcreteFactory));
  factory1->factory.createProduct = ConcreteFactory_createProduct;
  // 创建具体工厂
  AnotherConcreteFactory *factory2 = malloc(sizeof(AnotherConcreteFactory));
  factory2->factory.createProduct = AnotherConcreteFactory_createProduct;
  // 使用工厂创建产品
  Product *product1 = factory1->factory.createProduct();
  Product *product2 = factory2->factory.createProduct();
  // 调用产品的 print 方法
  product1->print(product1);
  product2->print(product2);
  // 释放内存
  free(product1);
  free(product2);
  free(factory1);
  free(factory2);

  return 0;
}
