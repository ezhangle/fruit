/*
 * Copyright 2014 Google Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FRUIT_COMPONENT_DEFN_H
#define FRUIT_COMPONENT_DEFN_H

#include "../component.h"

#include "injection_errors.h"
#include "storage/component_storage.h"

#include <memory>

namespace fruit {

template <typename... Params>
template <typename OtherComp>
inline Component<Params...>::Component(PartialComponent<OtherComp> component)
  : PartialComponent<fruit::impl::meta::Apply<fruit::impl::meta::ConstructComponentImpl, Params...>>(std::move(component)) {
}

inline Component<> createComponent() {
  return {};
}

template <typename Comp>
inline PartialComponent<Comp>::PartialComponent(fruit::impl::ComponentStorage&& storage)
  : storage(std::move(storage)) {
}

template <typename Comp>
template <typename SourceComp>
inline PartialComponent<Comp>::PartialComponent(PartialComponent<SourceComp> sourceComponent)
  : storage(std::move(sourceComponent.storage)) {
  using Op = fruit::impl::meta::Apply<
      fruit::impl::ComposeFunctors<
          fruit::impl::ProcessDeferredBindings,
          fruit::impl::ConvertComponent<Comp>>,
      SourceComp>;
  Op()(storage);
}

template <typename Comp>
template <typename I, typename C>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::AddDeferredInterfaceBinding<I, C>, Comp>::Result>
PartialComponent<Comp>::bind() && {
  FruitDelegateCheck(fruit::impl::NotABaseClassOf<I, C>);
  fruit::impl::meta::Apply<fruit::impl::AddDeferredInterfaceBinding<I, C>, Comp>()(storage);
  return {std::move(storage)};
}

template <typename Comp>
template <typename Signature>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::DeferredRegisterConstructor<Signature>, Comp>::Result>
PartialComponent<Comp>::registerConstructor() && {
  FruitDelegateCheck(fruit::impl::ParameterIsNotASignature<Signature>);
  FruitDelegateCheck(fruit::impl::ConstructorDoesNotExist<Signature>);
  fruit::impl::meta::Apply<fruit::impl::DeferredRegisterConstructor<Signature>, Comp>()(storage);
  return {std::move(storage)};
}

template <typename Comp>
template <typename C>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::RegisterInstance<C>, Comp>::Result>
PartialComponent<Comp>::bindInstance(C& instance) && {
  fruit::impl::meta::Apply<fruit::impl::RegisterInstance<C>, Comp>()(storage, instance);
  return {std::move(storage)};
}

template <typename Comp>
template <typename Function>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::DeferredRegisterProvider<Function>, Comp>::Result>
PartialComponent<Comp>::registerProvider(Function provider) && {
  (void)provider;
  fruit::impl::meta::Apply<fruit::impl::DeferredRegisterProvider<Function>, Comp>()(storage);
  return {std::move(storage)};
}

template <typename Comp>
template <typename I, typename C>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::AddInterfaceMultibinding<I, C>, Comp>::Result>
PartialComponent<Comp>::addMultibinding() && {
  FruitDelegateCheck(fruit::impl::NotABaseClassOf<I, C>);
  fruit::impl::meta::Apply<fruit::impl::AddInterfaceMultibinding<I, C>, Comp>()(storage);
  return {std::move(storage)};
}

template <typename Comp>
template <typename C>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::AddInstanceMultibinding<C>, Comp>::Result>
PartialComponent<Comp>::addInstanceMultibinding(C& instance) && {
  fruit::impl::meta::Apply<fruit::impl::AddInstanceMultibinding<C>, Comp>()(storage, instance);
  return {std::move(storage)};
}

template <typename Comp>
template <typename C>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::AddInstanceMultibindings<C>, Comp>::Result>
PartialComponent<Comp>::addInstanceMultibindings(std::vector<C>& instances) && {
  fruit::impl::meta::Apply<fruit::impl::AddInstanceMultibindings<C>, Comp>()(storage, instances);
  return {std::move(storage)};
}

template <typename Comp>
template <typename Function>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::RegisterMultibindingProvider<Function>, Comp>::Result>
PartialComponent<Comp>::addMultibindingProvider(Function provider) && {
  (void)provider;
  fruit::impl::meta::Apply<fruit::impl::RegisterMultibindingProvider<Function>, Comp>()(storage);
  return {std::move(storage)};
}
  
template <typename Comp>
template <typename AnnotatedSignature, typename Lambda>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::RegisterFactory<AnnotatedSignature, Lambda>, Comp>::Result>
PartialComponent<Comp>::registerFactory(Lambda) && {
  fruit::impl::meta::Apply<fruit::impl::RegisterFactory<AnnotatedSignature, Lambda>, Comp>()(storage);
  return {std::move(storage)};
}

template <typename Comp>
template <typename... OtherCompParams>
inline PartialComponent<typename fruit::impl::meta::Apply<fruit::impl::InstallComponentHelper<OtherCompParams...>, Comp>::Result>
PartialComponent<Comp>::install(Component<OtherCompParams...> component) && {
  fruit::impl::meta::Apply<fruit::impl::InstallComponentHelper<OtherCompParams...>, Comp>()(storage,
                                                                                             std::move(component.storage));
  return {std::move(storage)};
}

} // namespace fruit


#endif // FRUIT_COMPONENT_DEFN_H
