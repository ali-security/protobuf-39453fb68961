package com.google.protobuf;

import static com.google.common.truth.Truth.assertThat;

import test.TestImportJavaFeaturesProto.Item;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

// Test in isolation on purpose to ensure that DescriptorsProto and JavaFeaturesProto are not loaded
// or initialized
@RunWith(JUnit4.class)
public class ImportJavaFeaturesTest {

  @Test
  public void testImportAndSetJavaFeatures() throws Exception {
    Item item = Item.newBuilder().setName("foobar").build();

    assertThat(item.getName()).isEqualTo("foobar");
    assertThat(item.getDescriptorForType().getFieldCount()).isEqualTo(1);
    assertThat(item.toString()).contains("foobar");
  }
}
