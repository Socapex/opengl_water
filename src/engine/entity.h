#pragma once
#include "engine/globals.h"

#include <functional>
#include <memory>
#include <vector>

template <class T> struct Component;

struct Entity {
	static void* operator new(size_t) = delete;
	static void* operator new[](size_t) = delete;

	inline bool operator==(const Entity& e) const {
		return _id == e._id;
	}
	
	inline bool operator!=(const Entity& e) const {
		return _id != e._id;
	}
	
	inline friend bool operator<(const Entity& lhs, const Entity& rhs) {
		return lhs._id < rhs._id;
	}

	inline size_t id() const {
		return _id;
	}
	
	inline void debug_print() const {
		printf("%zu\n", _id);
	}

	template <class T> Component<T> add_component();
	template <class T> Component<T> get_component();
	template <class T> void kill_component();

	static Entity add_entity();
	static void kill_entity(Entity e);
	static void on_component_kill(std::function<void(Entity)>&& f);

	static const Entity dummy;//{ std::numeric_limits<size_t>::max() };

	// TODO: Enable / disable. Can just enable / disable all components.
private:
	Entity();
	Entity(size_t id);

	size_t _id;

	static size_t _id_count;
	static std::vector<Entity> _entities;
	static std::vector<std::function<void(Entity)>> _components_kill;
};

template <class T>
Component<T> Entity::add_component() {
	static_assert(std::is_base_of<Component<T>, T>::value
			, "Your component needs to inherit Component<>.");

	auto ret = get_component<T>();
	if (ret) {
		OUTPUT_ERROR("Can't add duplicate components in entity. "
				"Returning existing Component.");
		return ret;
	}

	return Component<T>::add_component(*this);
}

template <class T>
Component<T> Entity::get_component() {
	static_assert(std::is_base_of<Component<T>, T>::value,
			"Components must inherit Component<>.");

	return Component<T>{*this};
}

template <class T>
void Entity::kill_component() {
	static_assert(std::is_base_of<Component<T>, T>::value
			, "Your component needs to inherit Component<>.");

	Component<T>::kill_component(*this);
}
