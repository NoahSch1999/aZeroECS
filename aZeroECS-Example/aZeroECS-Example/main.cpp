#include <iostream>
#include "exampleSetup.h"

int main()
{
    constexpr int MAX_ENTITIES = 0;
    constexpr int RESIZE_AMOUNT = 1;

    aZero::ECS::EntityManager EManager;

    // Init declaration defined in "exampleSetup.h"
    ComponentManagerDecl CManager;

    // Initialize array with an amount of entities supported
    // Once an entities that has a higher ID than MAX_ENTITIES is requesting a component to be added, the array resizes with RESIZE_AMOUNT new elements
        // New supported MAX_ENTITIES is then internally MAX_ENTITIES + RESIZE_AMOUNT etc...
    CManager.GetComponentArray<ComponentThree>().Init(MAX_ENTITIES, RESIZE_AMOUNT);

    // Create Entity which has to be cached, or else lost in the sauce
    aZero::ECS::Entity EntityOneObj = EManager.CreateEntity();
    aZero::ECS::Entity EntityTwoObj = EManager.CreateEntity();

    // Add component of type "ComponentThree"
    //CManager.AddComponent<ComponentThree>(Entity); // Default initialized component
    CManager.AddComponent(EntityOneObj, ComponentThree(1337.0)); // Component initialized with specified data
    CManager.AddComponent(EntityTwoObj, ComponentThree(51.0)); // Component initialized with specified data

    // Get bit index for the component
    // Bit index is decided at compile time and corresponds to the template specialization order for the ComponentManager class
    int BitIndex = CManager.GetComponentBit<ComponentThree>(); // Will equal 3

    // Checks in the Entity bitmask if it has a component of type "ComponentThree"
    bool HasComponentThree = CManager.HasComponent<ComponentThree>(EntityOneObj); // Will be true

    // Get Component array which contains all the components of the template specialization types contiguously in memory
    //auto ComponentArrayThree = CManager.GetComponentArray<ComponentThree>();
    const aZero::ECS::ComponentArray<ComponentThree>& ComponentArrayThree = CManager.GetComponentArray<ComponentThree>();

    // Iterate over components
    for (const ComponentThree& Component : ComponentArrayThree.GetInternalArray())
    {
        std::cout << Component.x << "\n";
    }

    aZero::ECS::ComponentArray<ComponentThree>& ComponentArrayThreeRef = CManager.GetComponentArray<ComponentThree>();

    // ::GetComponent() returns nullptr if the ECS::Entity doesn't have a component of the type ComponentThree
    // The validity of the pointer can only be guaranteed until ECS::ComponentArray::AddComponent() or ECS::ComponentArray::RemoveComponent() is called
        // This is because the underlying memory might be reallocated because of resizing
    ComponentThree* ComponentPtr = ComponentArrayThreeRef.GetComponent(EntityOneObj);
    if (ComponentPtr)
    {
        ComponentPtr->x = 1.0;
    }

    for (const ComponentThree& Component : ComponentArrayThreeRef.GetInternalArray())
    {
        std::cout << Component.x << "\n";
    }

    // Remove component of type "ComponentThree"
    CManager.RemoveComponent<ComponentThree>(EntityOneObj);
    CManager.RemoveComponent<ComponentThree>(EntityTwoObj);
}