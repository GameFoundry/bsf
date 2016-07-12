//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//

using System;
using System.Collections.Generic;
using BansheeEditor.Windows.Inspector.Style;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Contains style information about inspectable items
    /// </summary>
    public class InspectableFieldStyle
    {
        /// <summary>
        /// Creates all the relevant style information for a SerializableField
        /// </summary>
        /// <param name="field">A SerializableField</param>
        /// <returns>Style information</returns>
        public static InspectableFieldStyle[] Create(SerializableField field)
        {
            List<InspectableFieldStyle> styles = new List<InspectableFieldStyle>();
            if (field.Range)
                styles.Add(new InspectableFieldRangeStyle(field.RangeMinimum, field.RangeMaximum));
            if (field.Step != default(float))
                styles.Add(new InspectableFieldStepStyle(field.Step));
            return styles.ToArray();
        }

        /// <summary>
        /// Finds a specific type of style information
        /// </summary>
        /// <typeparam name="T">The specified type</typeparam>
        /// <param name="styleInfo">The array to search into</param>
        /// <returns>The desired style or default</returns>
        public static T FindStyle<T>(InspectableFieldStyle[] styleInfo) where T : InspectableFieldStyle
        {
            for  (int i = 0; i < styleInfo.Length; i++)
            {
                if (styleInfo[i].GetType() == typeof(T))
                    return (T)styleInfo[i];
            }
            return default(T);
        }
    }
}
