#include "engine/entity.h"
#include "engine/engine.h"

#include <limits>

Entity::Entity()
	: _id(++_id_count)
{}

Entity::Entity(size_t id)
	: _id(id)
{}

Entity Entity::add_entity() {
	_entities.emplace_back(Entity{});
	_lut[_entities.back().id()] = _entities.size() - 1;
	return _entities.back();
}

void Entity::kill_entity(Entity e) {
	for (size_t i = 0; i < _kill_component_callback.size(); ++i) {
		_kill_component_callback[i](e);
	}

	size_t pos = _lut[e._id];
	if (_entities[pos]._id != _entities.back()._id) {
		_lut[_entities.back()._id] = pos;
		std::swap(_entities[pos], _entities.back());
	}
	_entities.pop_back();
	_lut.erase(e._id);
}

void Entity::on_component_kill(std::function<void(Entity)>&& f) {
	Entity::_kill_component_callback.emplace_back(f);
}

const Entity Entity::dummy{ std::numeric_limits<size_t>::max() };
size_t Entity::_id_count = 0;
std::unordered_map<size_t, size_t> Entity::_lut = {};
std::vector<Entity> Entity::_entities = {};
std::vector<std::function<void(Entity)>> Entity::_kill_component_callback = {};
