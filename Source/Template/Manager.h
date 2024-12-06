#pragma once
#include <vector>
#include <set>
template<class C>
class Manager
{
private:
	std::vector<C*> objs;
	std::set<C*> remove_objs;
public:
	Manager() = default;
	virtual ~Manager() = default;

	virtual void Update(float elapsedTime)
	{
		for (C* obj : objs) obj->Update(elapsedTime);
		for (auto&& erase : remove_objs)
		{
			auto&& it = std::find(objs.begin(), objs.end(), erase);
			if (it != objs.end()) objs.erase(it);
			delete(erase);
		}
		this->remove_objs.clear();
	}
	virtual void Render() { for (C* obj : objs) obj->Render(); }
	virtual void DrawGUI() { for (C* obj : objs) obj->DrawGUI(); }
	virtual void Clear()
	{
		for (C* obj : objs) delete obj;
		objs.clear();
	}
	void Register(C* obj) { objs.emplace_back(obj); }
	void Remove(C* obj) { remove_objs.insert(obj); }
	C* Get(int index) { return objs.at(index); }
	int GetCount() const { return static_cast<int>(objs.size()); }
};
