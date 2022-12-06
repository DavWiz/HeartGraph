﻿// Copyright Guy (Drakynfly) Lundvall. All Rights Reserved.

#pragma once

#include "UObject/Object.h"
#include "HeartGuids.h"
#include "HeartGraph.generated.h"

class UHeartGraphNode;
class UHeartGraphSchema;

DECLARE_LOG_CATEGORY_EXTERN(LogHeartGraph, Log, All)

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHeartGraphEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHeartGraphNodeEvent, UHeartGraphNode*, Node);

USTRUCT()
struct FHeartGraphSparseClassData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Display")
	FText GraphTypeName;

#if WITH_EDITORONLY_DATA
	// Can the editor create instances of this graph as an asset.
	UPROPERTY(EditDefaultsOnly, Category = "Editor")
	bool CanCreateAssetFromFactory = false;
#endif
};

/**
 *
 */
UCLASS(Abstract, BlueprintType, Blueprintable, SparseClassDataTypes = "HeartGraphSparseClassData")
class HEART_API UHeartGraph : public UObject
{
	GENERATED_BODY()

	friend class UHeartEdGraph;

public:

	virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;

#if WITH_EDITOR
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);
#endif

	/****************************/
	/**		GETTERS				*/
	/****************************/
public:

#if WITH_EDITOR
	UEdGraph* GetEdGraph() const { return HeartEdGraph; }
#endif

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	FHeartGraphGuid GetGuid() const { return Guid; }

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	UHeartGraphNode* GetNode(const FHeartNodeGuid& NodeGuid) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Heart|Graph")
	void GetNodeArray(TArray<UHeartGraphNode*>& OutNodes) const
	{
		// *le sign* epic templates mess this up . . .
		TArray<TObjectPtr<UHeartGraphNode>> NodeArray;
		Nodes.GenerateValueArray(NodeArray);
		OutNodes = NodeArray;
	}


	/****************************/
	/**		CLASS BEHAVIOR		*/
	/****************************/
public:
	/** Override to specify the behavior class for this graph class */
	UFUNCTION(BlueprintNativeEvent, Category = "Heart|Graph")
	TSubclassOf<UHeartGraphSchema> GetSchemaClass() const;

	template <typename THeartGraphSchema>
	const THeartGraphSchema* GetSchemaTyped() const
	{
		static_assert(TIsDerivedFrom<THeartGraphSchema, UHeartGraphSchema>::IsDerived, "The schema class must derive from UHeartGraphSchema");
		return Cast<THeartGraphSchema>(GetSchema());
	}

	static const UHeartGraphSchema* GetSchemaStatic(TSubclassOf<UHeartGraph> HeartGraphClass);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	const UHeartGraphSchema* GetSchema() const;

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph", meta = (DeterminesOutputType = Class), DisplayName = "Get Schema Typed")
	const UHeartGraphSchema* GetSchemaTyped_K2(TSubclassOf<UHeartGraphSchema> Class) const;


	/****************************/
	/**		NODE EDITING		*/
	/****************************/
private:
	UHeartGraphNode* CreateNodeForNodeObject(UObject* NodeObject, const FVector2D& Location);

	UHeartGraphNode* CreateNodeForNodeClass(const TSubclassOf<UObject> NodeClass, const FVector2D& Location);

public:
	// Create from template graph class and template node class
	template <typename THeartGraphNode, typename THeartNode>
	THeartGraphNode* CreateNode(const FVector2D& Location)
	{
		static_assert(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::IsDerived, "The graph node class must derive from UHeartGraphNode");
		static_assert(!TIsDerivedFrom<THeartNode, UHeartGraphNode>::IsDerived, "The node class must not derive from UHeartGraphNode");
		return Cast<THeartGraphNode>(CreateNodeForNodeClass(THeartNode::StaticClass(), Location));
	}

	// Create from template graph class and node class
	template <typename THeartGraphNode>
	THeartGraphNode* CreateNode(const TSubclassOf<UObject> NodeClass, const FVector2D& Location)
	{
		static_assert(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::IsDerived, "The graph node class must derive from UHeartGraphNode");
		checkf(!NodeClass->IsChildOf<THeartGraphNode>(), TEXT("If this trips, you've passed in a 'GRAPH' node class instead of a 'OBJECT' node class"));
		return Cast<THeartGraphNode>(CreateNodeForNodeClass(NodeClass, Location));
	}

	// Create from template graph class and node object
	template <typename THeartGraphNode>
	THeartGraphNode* CreateNode(UObject* NodeObject, const FVector2D& Location)
	{
		static_assert(TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::IsDerived, "The graph node class must derive from UHeartGraphNode");
		static_assert(!TIsDerivedFrom<THeartGraphNode, UHeartGraphNode>::IsDerived, "The node class must not derive from UHeartGraphNode");
		return Cast<THeartGraphNode>(CreateNodeForNodeObject(NodeObject, Location));
	}

	/**
	 * Create a new node, spawning a new NodeObject from the NodeClass provided.
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UHeartGraphNode* CreateNodeFromClass(const TSubclassOf<UObject> NodeClass, const FVector2D& Location);

	/**
	 * Create a new node, using the provided NodeObject
	 */
	UFUNCTION(BlueprintCallable, Category = "Heart|GraphNode")
	UHeartGraphNode* CreateNodeFromObject(UObject* NodeObject, const FVector2D& Location);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	void AddNode(UHeartGraphNode* Node);

	UFUNCTION(BlueprintCallable, Category = "Heart|Graph")
	bool RemoveNode(const FHeartNodeGuid& NodeGuid);


public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FHeartGraphNodeEvent OnNodeAdded;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FHeartGraphNodeEvent OnNodeRemoved;

#if WITH_EDITORONLY_DATA
private:
	// Always castable to UHeartEdGraph
	UPROPERTY()
	UEdGraph* HeartEdGraph;
#endif

private:
	UPROPERTY()
	FHeartGraphGuid Guid;

	UPROPERTY()
	TMap<FHeartNodeGuid, TObjectPtr<UHeartGraphNode>> Nodes;
};