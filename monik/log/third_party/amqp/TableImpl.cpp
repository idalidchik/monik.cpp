// TableImpl.cpp
//
#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#include "monik/log/third_party/amqp/TableImpl.h"

namespace AmqpClient { namespace Detail {

bool value_t::operator == (value_t const & v) const
{
    M_STATIC_ASSERT_IS_POD(value_t::union_t);
    MONIK_ASSERT(kind != TableValue::VT_void);
    MONIK_ASSERT(v.kind != TableValue::VT_void);
    if (kind != v.kind) return false;
    switch(kind) {
    case TableValue::VT_bool:      return (variant.boolean == v.variant.boolean);
    case TableValue::VT_int8:      return (variant.i8 == v.variant.i8);
    case TableValue::VT_int16:     return (variant.i16 == v.variant.i16);
    case TableValue::VT_int32:     return (variant.i32 == v.variant.i32);
    case TableValue::VT_int64:     return (variant.i64 == v.variant.i64);
    case TableValue::VT_float:     return (variant.f32 == v.variant.f32);
    case TableValue::VT_double:    return (variant.f64 == v.variant.f64);
    case TableValue::VT_string:    return (string_ == v.string_);
    case TableValue::VT_array:     return (array_ == v.array_);
    case TableValue::VT_table:     return (table_ == v.table_);
    default:
        MONIK_ASSERT(0);
        return false;
    }
}

amqp_field_value_t TableValueImpl::generate_field_value::operator()(
    const void_t) const {
  amqp_field_value_t v;
  v.kind = AMQP_FIELD_KIND_VOID;
  return v;
}

amqp_field_value_t TableValueImpl::generate_field_value::operator()(
    const bool value) const {
  amqp_field_value_t v;
  v.kind = AMQP_FIELD_KIND_BOOLEAN;
  v.value.boolean = value;
  return v;
}

amqp_field_value_t TableValueImpl::generate_field_value::operator()(
    const std::int8_t value) const {
  amqp_field_value_t v;
  v.kind = AMQP_FIELD_KIND_I8;
  v.value.i8 = value;
  return v;
}

amqp_field_value_t TableValueImpl::generate_field_value::operator()(
    const std::int16_t value) const {
  amqp_field_value_t v;
  v.kind = AMQP_FIELD_KIND_I16;
  v.value.i16 = value;
  return v;
}

amqp_field_value_t TableValueImpl::generate_field_value::operator()(
    const std::int32_t value) const {
  amqp_field_value_t v;
  v.kind = AMQP_FIELD_KIND_I32;
  v.value.i32 = value;
  return v;
}

amqp_field_value_t TableValueImpl::generate_field_value::operator()(
    const std::int64_t value) const {
  amqp_field_value_t v;
  v.kind = AMQP_FIELD_KIND_I64;
  v.value.i64 = value;
  return v;
}

amqp_field_value_t TableValueImpl::generate_field_value::operator()(
    const float value) const {
  amqp_field_value_t v;
  v.kind = AMQP_FIELD_KIND_F32;
  v.value.f32 = value;
  return v;
}

amqp_field_value_t TableValueImpl::generate_field_value::operator()(
    const double value) const {
  amqp_field_value_t v;
  v.kind = AMQP_FIELD_KIND_F64;
  v.value.f64 = value;
  return v;
}

amqp_field_value_t TableValueImpl::generate_field_value::operator()(
    const std::string &value) const {
  amqp_field_value_t v;
  v.kind = AMQP_FIELD_KIND_UTF8;
  amqp_pool_alloc_bytes(&pool, value.size(), &v.value.bytes);
  memcpy(v.value.bytes.bytes, value.data(), v.value.bytes.len);
  return v;
}

amqp_field_value_t TableValueImpl::generate_field_value::operator()(
    const value_t& value) const
{
    MONIK_ASSERT(value.kind != TableValue::VT_void);
    switch(value.kind) {
    case TableValue::VT_bool:      return operator()(value.variant.boolean);
    case TableValue::VT_int8:      return operator()(value.variant.i8);
    case TableValue::VT_int16:     return operator()(value.variant.i16);
    case TableValue::VT_int32:     return operator()(value.variant.i32);
    case TableValue::VT_int64:     return operator()(value.variant.i64);
    case TableValue::VT_float:     return operator()(value.variant.f32);
    case TableValue::VT_double:    return operator()(value.variant.f64);
    case TableValue::VT_string:    return operator()(value.string_);
    case TableValue::VT_array:     return operator()(value.array_);
    case TableValue::VT_table:     return operator()(value.table_);
    default:
        MONIK_ASSERT(0);
        return{};
    }
}

amqp_field_value_t TableValueImpl::generate_field_value::operator()(
    const array_t &value) const {
  amqp_field_value_t v;
  v.kind = AMQP_FIELD_KIND_ARRAY;
  v.value.array.num_entries = value.size();
  v.value.array.entries = (amqp_field_value_t *)amqp_pool_alloc(
      &pool, sizeof(amqp_field_value_t) * value.size());
  if (NULL == v.value.array.entries) {
    throw std::bad_alloc();
  }

  amqp_field_value_t *output_iterator = v.value.array.entries;
  for (array_t::const_iterator it = value.begin(); it != value.end(); ++it, ++output_iterator) {
    *output_iterator = generate_field_value(pool)(it->m_impl->m_value);
  }
  return v;
}

amqp_field_value_t TableValueImpl::generate_field_value::operator()(
    const Table &value) const {
  amqp_field_value_t v;
  v.kind = AMQP_FIELD_KIND_TABLE;
  v.value.table = CreateAmqpTableInner(value, pool);
  return v;
}

void free_pool(amqp_pool_t *pool) {
  empty_amqp_pool(pool);
  delete pool;
}

amqp_table_t TableValueImpl::CreateAmqpTable(const Table &table,
                                             amqp_pool_ptr_t &pool) {
  if (0 == table.size()) {
    return AMQP_EMPTY_TABLE;
  }

  pool = std::shared_ptr<amqp_pool_t>(new amqp_pool_t, free_pool);
  init_amqp_pool(pool.get(), 1024);

  return CreateAmqpTableInner(table, *pool.get());
}

amqp_table_t TableValueImpl::CreateAmqpTableInner(const Table &table,
                                                  amqp_pool_t &pool) {
  amqp_table_t new_table;

  new_table.num_entries = table.size();

  new_table.entries = (amqp_table_entry_t *)amqp_pool_alloc(
      &pool, sizeof(amqp_table_entry_t) * table.size());

  if (NULL == new_table.entries) {
    throw std::bad_alloc();
  }

  amqp_table_entry_t *output_it = new_table.entries;

  for (Table::const_iterator it = table.begin(); it != table.end();
       ++it, ++output_it) {
    amqp_pool_alloc_bytes(&pool, it->first.size(), &output_it->key);
    if (NULL == output_it->key.bytes) {
      throw std::bad_alloc();
    }

    std::copy(it->first.begin(), it->first.end(), (char *)output_it->key.bytes);
    output_it->value = TableValueImpl::generate_field_value(pool)(it->second.m_impl->m_value);
  }

  return new_table;
}

Table TableValueImpl::CreateTable(const amqp_table_t &table) {
  Table new_table;

  for (int i = 0; i < table.num_entries; ++i) {
    amqp_table_entry_t *entry = &table.entries[i];

    std::string key((char *)entry->key.bytes, entry->key.len);

    new_table.insert(TableEntry(key, CreateTableValue(entry->value)));
  }
  return new_table;
}

TableValue TableValueImpl::CreateTableValue(const amqp_field_value_t &entry) {
  switch (entry.kind) {
    case AMQP_FIELD_KIND_VOID:
      return TableValue();
    case AMQP_FIELD_KIND_BOOLEAN:
      return TableValue((bool)entry.value.boolean);
    case AMQP_FIELD_KIND_I8:
      return TableValue(entry.value.i8);
    case AMQP_FIELD_KIND_I16:
      return TableValue(entry.value.i16);
    case AMQP_FIELD_KIND_I32:
      return TableValue(entry.value.i32);
    case AMQP_FIELD_KIND_I64:
    case AMQP_FIELD_KIND_TIMESTAMP:
      return TableValue(entry.value.i64);
    case AMQP_FIELD_KIND_F32:
      return TableValue(entry.value.f32);
    case AMQP_FIELD_KIND_F64:
      return TableValue(entry.value.f64);
    case AMQP_FIELD_KIND_UTF8:
    case AMQP_FIELD_KIND_BYTES:
      return TableValue(
          std::string((char *)entry.value.bytes.bytes, entry.value.bytes.len));
    case AMQP_FIELD_KIND_ARRAY: {
      amqp_array_t array = entry.value.array;
      Detail::array_t new_array;

      for (int i = 0; i < array.num_entries; ++i) {
        new_array.push_back(CreateTableValue(array.entries[i]));
      }

      return TableValue(new_array);
    }
    case AMQP_FIELD_KIND_TABLE:
      return TableValue(CreateTable(entry.value.table));
    case AMQP_FIELD_KIND_DECIMAL:
    default:
      return TableValue();
  }
}

amqp_table_t TableValueImpl::CopyTable(const amqp_table_t &table,
                                       amqp_pool_ptr_t &pool) {
  if (0 == table.num_entries) {
    return AMQP_EMPTY_TABLE;
  }

  pool = std::shared_ptr<amqp_pool_t>(new amqp_pool_t, free_pool);
  init_amqp_pool(pool.get(), 1024);

  return CopyTableInner(table, *pool.get());
}

amqp_table_t TableValueImpl::CopyTableInner(const amqp_table_t &table,
                                            amqp_pool_t &pool) {
  amqp_table_t new_table;

  new_table.num_entries = table.num_entries;
  new_table.entries = (amqp_table_entry_t *)amqp_pool_alloc(
      &pool, sizeof(amqp_table_entry_t) * table.num_entries);
  if (NULL == new_table.entries) {
    throw std::bad_alloc();
  }

  for (int i = 0; i < table.num_entries; ++i) {
    amqp_table_entry_t *entry = &new_table.entries[i];
    amqp_pool_alloc_bytes(&pool, table.entries[i].key.len, &entry->key);

    if (NULL == entry->key.bytes) {
      throw std::bad_alloc();
    }
    memcpy(entry->key.bytes, table.entries[i].key.bytes, entry->key.len);

    entry->value = CopyValue(table.entries[i].value, pool);
  }

  return new_table;
}

amqp_field_value_t TableValueImpl::CopyValue(const amqp_field_value_t value,
                                             amqp_pool_t &pool) {
  amqp_field_value_t new_value = value;

  switch (value.kind) {
    case AMQP_FIELD_KIND_UTF8:
    case AMQP_FIELD_KIND_BYTES:
      amqp_pool_alloc_bytes(&pool, value.value.bytes.len,
                            &new_value.value.bytes);
      memcpy(new_value.value.bytes.bytes, value.value.bytes.bytes,
             value.value.bytes.len);
      return new_value;
    case AMQP_FIELD_KIND_ARRAY: {
      new_value.value.array.entries = (amqp_field_value_t *)amqp_pool_alloc(
          &pool, sizeof(amqp_field_value_t) * value.value.array.num_entries);
      for (int i = 0; i < value.value.array.num_entries; ++i) {
        new_value.value.array.entries[i] =
            CopyValue(value.value.array.entries[i], pool);
      }
      return new_value;
    }
    case AMQP_FIELD_KIND_TABLE:
      new_value.value.table = CopyTableInner(value.value.table, pool);
      return new_value;
    default:
      return new_value;
  }
}
}
}  // namespace AmqpClient
